#include "common_definitions.h"
#include "common_algorithms.h"
#include "parameters.h"
#include "common.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TChain.h"
#include "TH1D.h"
#include "TSpline.h"
#include "TF1.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

TGraph* BuildCDF(TGraph *g_df)
{
	TGraph *g_cdf = new TGraph();
	g_cdf->SetName("g_cdf");

	double *X = g_df->GetX();
	double *Y = g_df->GetY();

	double S = 0.;
	for (int i = 0; i < g_df->GetN(); i++)
	{
		g_cdf->SetPoint(i, X[i], S);

		if (i != g_df->GetN() - 1)
		{
			double w = X[i+1] - X[i];
			S += (Y[i+1] + Y[i]) / 2. * w;
		}
	}

	return g_cdf;
}

//----------------------------------------------------------------------------------------------------

TGraph* BuildICDF(TGraph *g_cdf, double t_min, double t_max)
{
	printf(">> BuildICDF\n");

	// prepare normalized inverse c.d.f
	int i_min = 0, i_max = g_cdf->GetN() - 1;	// default index range
	for (int i = 0; i < g_cdf->GetN(); i++)
	{
		double x, y;
		g_cdf->GetPoint(i, x, y);
		if (x < t_min)
			i_min = i;
		if (x > t_max)
		{
			i_max = i;
			break;
		}
	}

	double p_min = 0., p_max = 0.;
	double t_min_real = 0., t_max_real = 0.;
	g_cdf->GetPoint(i_min, t_min_real, p_min);
	g_cdf->GetPoint(i_max, t_max_real, p_max);

	printf("\tpoints loaded: %i\n", i_max - i_min + 1);
	printf("\t\tfirst point: idx = %i, |t| = %.2E GeV^2, p = %.2E mb\n", i_min, t_min_real, p_min);
	printf("\t\tlast point: idx = %i, |t| = %.2E GeV^2, p = %.2E mb\n", i_max, t_max_real, p_max);
	printf("\t|t| range: %.2E to %.2E GeV^2\n", t_min_real, t_max_real);
	printf("\tcorresponding cross-section: %.2E mb\n", p_max - p_min);

	if (i_min >= i_max)
		throw "BuildICDF: i_min >= i_max.";

	TGraph *g_icdf = new TGraph();
	g_icdf->SetName("g_icdf");
	for (int i = i_min; i <= i_max; i++)
	{
		double x, y;
		g_cdf->GetPoint(i, x, y);
		g_icdf->SetPoint(g_icdf->GetN(), (y - p_min) / (p_max - p_min), x);
	}

	return g_icdf;
}

//----------------------------------------------------------------------------------------------------

TSpline* BuildSpline(TGraph *g)
{
	TSpline3 *s = new TSpline3("", g->GetX(), g->GetY(), g->GetN());
	s->SetName(g->GetName());
	return s;
}

//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	// defaults
	unsigned long n_events = (unsigned long) 4E7;
	string dsdt_model_file = "/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/combined/first_fits/2/do_fit.root";
	string dsdt_model_object = "variant 2/g_dsdt_CH";
	unsigned int seed = 1;
	double de_si_vtx = 50E-3;

	// parse command line
	if (argc != 2)
	{
		printf("ERROR: missing argument (diagonal).\n");
		return 1;
	}

	Init(argv[1]);
	if (diagonal == dUnknown)
	{
		printf("ERROR: unknown diagonal %s\n", argv[1]);
		return 1;
	}

	if (diagonal == dCombined || diagonal == ad45b_56b || diagonal == ad45t_56t)
		return rcIncompatibleDiagonal;

	// print parameters
	printf("n_events = %lu\n", n_events);
	printf("dsdt_model_file = %s\n", dsdt_model_file.c_str());
	printf("dsdt_model_object = %s\n", dsdt_model_object.c_str());
	printf("seed = %u\n", seed);
	printf("de_si_vtx = %E mm\n", de_si_vtx);

	// prevent overwriting experimental data
	if (!simulated_dataset)
	{
		printf("ERROR: this dataset is not for simulation.\n");
		return 50;
	}

	// print info
	printf("\n");
	printf("------------------------------ environment ------------------------------\n");
	env.Print();
	printf("\n");
	printf("------------------------------- analysis --------------------------------\n");
	anal.Print();
	printf("\n");

	// load input - dsigma/dt model
	TFile *f_in_dsdt = TFile::Open(dsdt_model_file.c_str());
	if (f_in_dsdt == NULL)
	{
		printf("ERROR: can't open file '%s'.\n", dsdt_model_file.c_str());
		return 1;
	}

	TGraph *g_dsdt = (TGraph *) f_in_dsdt->Get(dsdt_model_object.c_str());
	if (g_dsdt == NULL)
	{
		printf("ERROR: can't load object '%s'.\n", dsdt_model_object.c_str());
		return 2;
	}

	TGraph *g_df = new TGraph(*g_dsdt);
	g_df->SetName("g_df");

	TGraph *g_cdf = BuildCDF(g_df);

	// t_min ~ 8E-4 GeV^2 --> theta_min ~ 4.3 urad
	// theta_min - 4 * si(m_x) ~ 4.3 - 4*0.33 ~ 3 urad --> t ~ 3.8E-4 GeV^2
	TGraph *g_icdf = BuildICDF(g_cdf, 3.8E-4, anal.t_max_full);

	TSpline *s_icdf = BuildSpline(g_icdf);

	delete f_in_dsdt;

	// get th_y* dependent efficiency correction
	TF1 *f_3outof4_efficiency_L_2_F = NULL;
	TF1 *f_3outof4_efficiency_R_2_F = NULL;
	if (anal.use_3outof4_efficiency_fits)
	{
		string path = "/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/beta2500/2rp/efficiency_fits/global_fit.root";
		TFile *effFile = TFile::Open(path.c_str());
		if (!effFile)
			printf("ERROR: 3-out-of-4 efficiency file `%s' cannot be opened.\n", path.c_str());

		string diagonal = argv[1];
		f_3outof4_efficiency_L_2_F = (TF1 *) effFile->Get( (diagonal + "/L_2_F/pol1").c_str() );
		f_3outof4_efficiency_R_2_F = (TF1 *) effFile->Get( (diagonal + "/R_2_F/pol1").c_str() );

		printf("\n>> using 3-out-of-4 fits: %p, %p\n",
			f_3outof4_efficiency_L_2_F, f_3outof4_efficiency_R_2_F);

		if (!f_3outof4_efficiency_L_2_F || !f_3outof4_efficiency_R_2_F)
		{
			printf("ERROR: some of the 3-out-of-4 graphs can not be loaded.\n");
			return 100;
		}
	}

	// simulate vertex growth
	const double si_vtx_x_beg = env.si_vtx_x - de_si_vtx;
	const double si_vtx_x_end = env.si_vtx_x + de_si_vtx;

	const double si_vtx_y_beg = env.si_vtx_y - de_si_vtx;
	const double si_vtx_y_end = env.si_vtx_y + de_si_vtx;

	printf("\n");
	printf("si_vtx_x_beg = %E, si_vtx_x_end = %E\n", si_vtx_x_beg, si_vtx_x_end);
	printf("si_vtx_y_beg = %E, si_vtx_y_end = %E\n", si_vtx_y_beg, si_vtx_y_end);

	// ouput file
	string fn_out = string("distill_") + argv[1] + ".root";
	TFile *f_out = TFile::Open(fn_out.c_str(), "recreate");

	// set up output tree
	EventRed ev;
	TTree *outT = new TTree("distilled", "bla");

	outT->Branch("v_L_1_F", &ev.h.L_1_F.v); outT->Branch("x_L_1_F", &ev.h.L_1_F.x); outT->Branch("y_L_1_F", &ev.h.L_1_F.y);
	outT->Branch("v_L_2_F", &ev.h.L_2_F.v); outT->Branch("x_L_2_F", &ev.h.L_2_F.x); outT->Branch("y_L_2_F", &ev.h.L_2_F.y);

	outT->Branch("v_R_1_F", &ev.h.R_1_F.v); outT->Branch("x_R_1_F", &ev.h.R_1_F.x); outT->Branch("y_R_1_F", &ev.h.R_1_F.y);
	outT->Branch("v_R_2_F", &ev.h.R_2_F.v); outT->Branch("x_R_2_F", &ev.h.R_2_F.x); outT->Branch("y_R_2_F", &ev.h.R_2_F.y);

	outT->Branch("timestamp", &ev.timestamp);
	outT->Branch("run_num", &ev.run_num);
	outT->Branch("bunch_num", &ev.bunch_num);
	outT->Branch("event_num", &ev.event_num);
	outT->Branch("trigger_num", &ev.trigger_num);
	outT->Branch("trigger_bits", &ev.trigger_bits);

	// set seed
	gRandom->SetSeed(seed);

	// loop over the chain entries
	for (unsigned int long evi = 0; evi < n_events; evi++)
	{
		// generate timestamp
		double event_ratio = double(evi) / double(n_events);
		unsigned int timestamp = timestamp_min + event_ratio * (timestamp_max - timestamp_min);

		// update time-dependent parameters
		env.si_vtx_x = si_vtx_x_beg + event_ratio * (si_vtx_x_end - si_vtx_x_beg);
		env.si_vtx_y = si_vtx_y_beg + event_ratio * (si_vtx_y_end - si_vtx_y_beg);

		// generate true event
		Kinematics k_tr;
		k_tr.t = s_icdf->Eval(gRandom->Rndm());
		k_tr.phi = gRandom->Rndm() * M_PI * th_y_sign;

		k_tr.TPhiToThetas(env);

		// generate beam smearing: vertex and divergence
		Kinematics k_bsm = k_tr;

		k_bsm.vtx_x = gRandom->Gaus() * env.si_vtx_x;
		k_bsm.vtx_y = gRandom->Gaus() * env.si_vtx_y;

		k_bsm.th_x_L += gRandom->Gaus() * env.si_th_x_L;
		k_bsm.th_x_R += gRandom->Gaus() * env.si_th_x_R;

		k_bsm.th_y_L += gRandom->Gaus() * env.si_th_y_L;
		k_bsm.th_y_R += gRandom->Gaus() * env.si_th_y_R;

		// transport protons
		HitData h_id = ProtonTransport(k_bsm, env);

		// apply detector smearing
		ev.h.L_2_F.v = true;
		ev.h.L_2_F.x = h_id.L_2_F.x + gRandom->Gaus() * env.si_de_P_L;
		ev.h.L_2_F.y = h_id.L_2_F.y + gRandom->Gaus() * env.si_de_P_L;

		ev.h.R_2_F.v = true;
		ev.h.R_2_F.x = h_id.R_2_F.x + gRandom->Gaus() * env.si_de_P_R;
		ev.h.R_2_F.y = h_id.R_2_F.y + gRandom->Gaus() * env.si_de_P_R;

		ev.h.L_1_F.v = false;
		ev.h.L_1_F.x = 0.;
		ev.h.L_1_F.y = 0.;

		ev.h.R_1_F.v = false;
		ev.h.R_1_F.x = 0.;
		ev.h.R_1_F.y = 0.;

		// set (fake) meta data
		ev.timestamp = timestamp;
		ev.run_num = 0;
		ev.bunch_num = 0;
		ev.event_num = 0;
		ev.trigger_num = 0;
		ev.trigger_bits = 0;

		// simulate reconstruction
		Kinematics k_re = DoReconstruction(ev.h, env);

		// cut off low-|t| events
		if (k_re.t < 6E-4)
			continue;

		// determine inefficiencies
		double inefficiency_3outof4 = anal.inefficiency_3outof4;
		if (anal.use_3outof4_efficiency_fits)
		{
			const double th_y_abs = fabs(k_re.th_y);

			inefficiency_3outof4 = 0.;
			inefficiency_3outof4 += 1. - f_3outof4_efficiency_L_2_F->Eval(th_y_abs);
			inefficiency_3outof4 += 1. - f_3outof4_efficiency_R_2_F->Eval(th_y_abs);
		}

		const double inefficiency_Noutof4 = inefficiency_3outof4 + anal.inefficiency_shower_near;

		// apply inefficiencies
		if (gRandom->Rndm() < inefficiency_Noutof4)
			continue;

		// write data
		outT->Fill();
	}

	// save output
	gDirectory = f_out->mkdir("model");
	g_df->Write();
	g_cdf->Write();
	g_icdf->Write();

	gDirectory = f_out;
	outT->Write();

	delete f_out;
	return 0;
}
