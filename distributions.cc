#include "common_definitions.h"
#include "common_algorithms.h"
#include "AcceptanceCalculator.h"
#include "parameters.h"
#include "common.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TChain.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TMath.h"
#include "TF1.h"
#include "TTree.h"

#include <cmath>

using namespace std;

//----------------------------------------------------------------------------------------------------

void ProfileToRMSGraph(TProfile *p, TGraphErrors *g)
{
	for (int bi = 1; bi <= p->GetNbinsX(); ++bi)
	{
		double c = p->GetBinCenter(bi);

		double N = p->GetBinEntries(bi);
		double Sy = p->GetBinContent(bi) * N;
		double Syy = p->GetSumw2()->At(bi);

		double si_sq = Syy/N - Sy*Sy/N/N;
		double si = (si_sq >= 0.) ? sqrt(si_sq) : 0.;
		double si_unc_sq = si_sq / 2. / N;	// Gaussian approximation
		double si_unc = (si_unc_sq >= 0.) ? sqrt(si_unc_sq) : 0.;

		int idx = g->GetN();
		g->SetPoint(idx, c, si);
		g->SetPointError(idx, 0., si_unc);
	}
}

//----------------------------------------------------------------------------------------------------

TGraphErrors* MakeRelDiff(TH1D *input)
{
	TGraphErrors *g_rel_diff = new TGraphErrors();

	for (int i = 1; i <= input->GetNbinsX(); ++i)
	{
		double t = input->GetBinCenter(i);
		double v = input->GetBinContent(i);
		double v_u = input->GetBinError(i);

		if (t > 0.4)
			continue;

		double ref = 530. * exp(-19.6*t);

		double rv = v / ref - 1.;
		double rv_u = v_u / ref;

		int idx = g_rel_diff->GetN();
		g_rel_diff->SetPoint(idx, t, rv);
		g_rel_diff->SetPointError(idx, 0., rv_u);
	}

	return g_rel_diff;
}

//----------------------------------------------------------------------------------------------------

unsigned int SuppressLowStatisticsBins(TProfile *p, int threshold)
{
	unsigned int reasonableBins = 0;
	for (int bi = 1; bi <= p->GetNbinsX(); ++bi)
	{
		if (p->GetBinEntries(bi) < threshold)
		{
			p->SetBinContent(bi, 0.);
			p->SetBinError(bi, 0.);
		} else
			reasonableBins++;
	}

	return reasonableBins;
}

//----------------------------------------------------------------------------------------------------

void HideLowTBins(TH1D *h, double threshold)
{
	for (int bi = 1; bi <= h->GetNbinsX(); ++bi)
	{
		if (h->GetBinCenter(bi) < threshold)
		{
			h->SetBinContent(bi, 0.);
			h->SetBinError(bi, 0.);
		}
	}
}

//----------------------------------------------------------------------------------------------------

TGraph* PlotFiductialCut(const FiducialCut &fc, double th_y_sign)
{
	TGraph *g = new TGraph();

	vector<double> th_x_values = { fc.th_x_m - 200E-6, fc.th_x_m, fc.th_x_p, fc.th_x_p + 200E-6 };

	for (const double &th_x : th_x_values)
	{
		g->SetPoint(g->GetN(), th_x, th_y_sign * fc.GetThYLimit(th_x));
	}

	return g;
}

//----------------------------------------------------------------------------------------------------

struct OpticsMatchingInput
{
	TGraph *g_thl_x_R_vs_thl_x_L;
	TGraph *g_y_R_220_F_vs_y_L_220_F;
	TGraph *g_thl_y_R_vs_y_R_220_F;
	TGraph *g_thl_y_L_vs_y_L_220_F;
	TGraph *g_thl_x_R_vs_x_R_220_F;
	TGraph *g_thl_x_L_vs_x_L_220_F;
	TGraph *g_x_L_220_F_vs_y_L_220_F;
	TGraph *g_x_L_210_F_vs_y_L_210_F;
	TGraph *g_x_R_210_F_vs_y_R_210_F;
	TGraph *g_x_R_220_F_vs_y_R_220_F;

	TGraph *g_vtx_x_L_vs_vtx_x_R;

	OpticsMatchingInput()
	{
		// constraints from New Journal of Physics 16 (2014) 103041

		// R1 = theta_x_loc_R / theta_x_loc_L
		g_thl_x_R_vs_thl_x_L = new TGraph(); g_thl_x_R_vs_thl_x_L->SetName("g_thl_x_R_vs_thl_x_L"); g_thl_x_R_vs_thl_x_L->SetTitle(";#theta_{x,loc}^{L}   (rad);#theta_{x,loc}^{R}   (rad)");

		// R2 = y_R_220_F / y_L_220_F
		g_y_R_220_F_vs_y_L_220_F = new TGraph(); g_y_R_220_F_vs_y_L_220_F->SetName("g_y_R_220_F_vs_y_L_220_F"); g_y_R_220_F_vs_y_L_220_F->SetTitle(";y^{L-220-F}   (mm);y^{R-220-F}   (mm)");

		// R3 = theta_y_loc_R / y_R_220_F
		g_thl_y_R_vs_y_R_220_F = new TGraph(); g_thl_y_R_vs_y_R_220_F->SetName("g_thl_y_R_vs_y_R_220_F"); g_thl_y_R_vs_y_R_220_F->SetTitle(";y^{R-220-F}   (mm);#theta_{y,loc}^{R}   (rad)");

		// R4 = theta_y_loc_L / y_L_220_F
		g_thl_y_L_vs_y_L_220_F = new TGraph(); g_thl_y_L_vs_y_L_220_F->SetName("g_thl_y_L_vs_y_L_220_F"); g_thl_y_L_vs_y_L_220_F->SetTitle(";y^{L-220-F}   (mm);#theta_{y,loc}^{L}   (rad)");

		// R5 = theta_x_loc_R / x_R_220_F
		g_thl_x_R_vs_x_R_220_F = new TGraph(); g_thl_x_R_vs_x_R_220_F->SetName("g_thl_x_R_vs_x_R_220_F"); g_thl_x_R_vs_x_R_220_F->SetTitle(";x^{R-220-F}   (mm);#theta_{x,loc}^{R}   (rad)");

		// R6 = theta_x_loc_L / x_L_220_F
		g_thl_x_L_vs_x_L_220_F = new TGraph(); g_thl_x_L_vs_x_L_220_F->SetName("g_thl_x_L_vs_x_L_220_F"); g_thl_x_L_vs_x_L_220_F->SetTitle(";x^{L-220-F}   (mm);#theta_{x,loc}^{L}   (rad)");

		// R7 = x_L_220_F / y_L_220_F
		g_x_L_220_F_vs_y_L_220_F = new TGraph(); g_x_L_220_F_vs_y_L_220_F->SetName("g_x_L_220_F_vs_y_L_220_F"); g_x_L_220_F_vs_y_L_220_F->SetTitle(";y^{L-220-F}   (mm);x^{L-220-F}   (mm)");

		// R8 = x_L_210_F / y_L_210_F
		g_x_L_210_F_vs_y_L_210_F = new TGraph(); g_x_L_210_F_vs_y_L_210_F->SetName("g_x_L_210_F_vs_y_L_210_F"); g_x_L_210_F_vs_y_L_210_F->SetTitle(";y^{L-210-F}   (mm);x^{L-210-F}   (mm)");

		// R9 = x_R_210_F / y_R_210_F
		g_x_R_210_F_vs_y_R_210_F = new TGraph(); g_x_R_210_F_vs_y_R_210_F->SetName("g_x_R_210_F_vs_y_R_210_F"); g_x_R_210_F_vs_y_R_210_F->SetTitle(";y^{R-210-F}   (mm);x^{R-210-F}   (mm)");

		// R10 = x_R_220_F / y_R_220_F
		g_x_R_220_F_vs_y_R_220_F = new TGraph(); g_x_R_220_F_vs_y_R_220_F->SetName("g_x_R_220_F_vs_y_R_220_F"); g_x_R_220_F_vs_y_R_220_F->SetTitle(";y^{R-220-F}   (mm);x^{R-220-F}   (mm)");

		// control plot
		g_vtx_x_L_vs_vtx_x_R = new TGraph(); g_vtx_x_L_vs_vtx_x_R->SetName("g_vtx_x_L_vs_vtx_x_R"); g_vtx_x_L_vs_vtx_x_R->SetTitle(";vtx_x^{R}   (mm);vtx_x^{L}   (mm)");
	}

	void Fill(const HitData &h_al, double thl_x_L, double thl_x_R, double thl_y_L, double thl_y_R, double vtx_x_L, double vtx_x_R)
	{
		int idx = g_thl_y_L_vs_y_L_220_F->GetN();

		if (idx < 10000)
		{
			g_thl_x_R_vs_thl_x_L->SetPoint(idx, thl_x_L, thl_x_R);
			g_y_R_220_F_vs_y_L_220_F->SetPoint(idx, h_al.L_2_F.y, h_al.R_2_F.y);

			g_thl_y_R_vs_y_R_220_F->SetPoint(idx, h_al.R_2_F.y, thl_y_R);
			g_thl_y_L_vs_y_L_220_F->SetPoint(idx, h_al.L_2_F.y, thl_y_L);

			g_thl_x_R_vs_x_R_220_F->SetPoint(idx, h_al.R_2_F.x, thl_x_R);
			g_thl_x_L_vs_x_L_220_F->SetPoint(idx, h_al.L_2_F.x, thl_x_L);

			g_x_L_220_F_vs_y_L_220_F->SetPoint(idx, h_al.L_2_F.y, h_al.L_2_F.x);
			g_x_L_210_F_vs_y_L_210_F->SetPoint(idx, h_al.L_1_F.y, h_al.L_1_F.x);
			g_x_R_210_F_vs_y_R_210_F->SetPoint(idx, h_al.R_1_F.y, h_al.R_1_F.x);
			g_x_R_220_F_vs_y_R_220_F->SetPoint(idx, h_al.R_2_F.y, h_al.R_2_F.x);

			g_vtx_x_L_vs_vtx_x_R->SetPoint(idx, vtx_x_R, vtx_x_L);
		}
	}

	void Write() const
	{
		g_thl_x_R_vs_thl_x_L->Write();
		g_y_R_220_F_vs_y_L_220_F->Write();

		g_thl_y_R_vs_y_R_220_F->Write();
		g_thl_y_L_vs_y_L_220_F->Write();

		g_thl_x_R_vs_x_R_220_F->Write();
		g_thl_x_L_vs_x_L_220_F->Write();

		g_x_L_220_F_vs_y_L_220_F->Write();
		g_x_L_210_F_vs_y_L_210_F->Write();
		g_x_R_210_F_vs_y_R_210_F->Write();
		g_x_R_220_F_vs_y_R_220_F->Write();

		g_vtx_x_L_vs_vtx_x_R->Write();
	}
};


//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if (argc < 2)
		return 1;

	// init diagonal settings
	Init(argv[1]);
	if (diagonal == dCombined || diagonal == ad45b_56b || diagonal == ad45t_56t)
		return rcIncompatibleDiagonal;

	// default parameters
	unsigned int detailsLevel = 0; 	// 0: no details, 1: some details, >= 2 all details
	bool overrideCutSelection = false;	// whether the default cut selection should be overriden by the command-line selection
	string cutSelectionString;
	string outputDir = ".";
	string inputDir = ".";
	double input_n_si = 4.0;
	int time_group_divisor = 0;
	int time_group_remainder = 0;
	int event_group_divisor = 0;
	int event_group_index = 0;
	unsigned int evIdxStep = 1;
	unsigned int maxTaggedEvents = 0;	// 0 means no maximum

	// parse command line arguments, starting from index 2
	for (int i = 2; i < argc; i++)
	{
		//printf("%u => %s\n", i, argv[i]);

		if (strcmp(argv[i], "-no-details") == 0)
		{
			detailsLevel = 0;
			continue;
		}

		if (strcmp(argv[i], "-details") == 0)
		{
			if (argc-1 > i)
				detailsLevel = atoi(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-cuts") == 0)
		{
			if (argc-1 > i)
			{
				cutSelectionString = argv[++i];
				overrideCutSelection = true;
			}
			continue;
		}

		if (strcmp(argv[i], "-output-dir") == 0)
		{
			if (argc-1 > i)
				outputDir = argv[++i];
			continue;
		}

		if (strcmp(argv[i], "-input-dir") == 0)
		{
			if (argc-1 > i)
				inputDir = argv[++i];
			continue;
		}

		if (strcmp(argv[i], "-n-si") == 0)
		{
			if (argc-1 > i)
				input_n_si = atof(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-tg-divisor") == 0)
		{
			if (argc-1 > i)
				time_group_divisor = atoi(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-tg-remainder") == 0)
		{
			if (argc-1 > i)
				time_group_remainder = atoi(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-eg-divisor") == 0)
		{
			if (argc-1 > i)
				event_group_divisor = (int) atof(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-eg-index") == 0)
		{
			if (argc-1 > i)
				event_group_index = (int) atof(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-step") == 0)
		{
			if (argc-1 > i)
				evIdxStep = (int) atof(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-events") == 0)
		{
			if (argc-1 > i)
				maxTaggedEvents = (int) atof(argv[++i]);
			continue;
		}

		printf("ERROR: unknown parameter `%s'.\n", argv[i]);
		return 3;
	}

	printf("* detailsLevel = %u\n", detailsLevel);
	printf("* outputDir = %s\n", outputDir.c_str());
	printf("* inputDir = %s\n", inputDir.c_str());
	printf("* input n_si = %.3f\n", input_n_si);
	printf("* time_group_divisor = %i\n", time_group_divisor);
	printf("* time_group_remainder = %i\n", time_group_remainder);
	printf("* event_group_divisor = %i\n", event_group_divisor);
	printf("* event_group_index = %i\n", event_group_index);
	printf("* evIdxStep = %u\n", evIdxStep);
	printf("* maxTaggedEvents = %u\n", maxTaggedEvents);

	// select cuts
	anal.BuildCuts(); 
	anal.n_si = input_n_si;

	if (overrideCutSelection)
	{
		anal.cuts.clear();
		char buf[100];
		strcpy(buf, cutSelectionString.c_str());

		// workaround for a Bash bug? --> keeping quotes around the parameter
		for (unsigned int i = 0; i < strlen(buf); i++)
			if (buf[i] == '"')
				buf[i] = ' ';

		printf("* setting cuts from selection string [%s]\n", buf);

		if (strcmp(buf, "  ") == 0 || strlen(buf) == 0)
			printf("* cut selection list empty\n");
		else {
			char *pch = strtok(buf, ",");
			while (pch != NULL)
			{
				unsigned int cut = atoi(pch);
				if (cut < 1 || cut > 9)
				{
					printf("ERROR: invalid cut number %u.\n", cut);
					return 2;
				}
				anal.cuts.push_back(cut);
				pch = strtok (NULL, ",");
			}
		}
	}

	// print info
	printf("\n");
	printf("------------------------------ environment ------------------------------\n");
	env.Print();
	printf("\n");
	printf("------------------------------- analysis --------------------------------\n");
	anal.Print();
	printf("\n");

	// alignment init
	for (unsigned int i = 0; i < alignmentSources.size(); ++i)
	{
		printf("\n---------- alignment source %u ----------\n", i);
		alignmentSources[i].Init();
	}
	printf("\n\n");

	// binnings
	vector<string> binnings;
	//binnings.push_back("ub");
	binnings.push_back("ob-1-20-0.05");
	binnings.push_back("ob-2-10-0.05");
	binnings.push_back("ob-3-5-0.05");

	// initialise acceptance calculation
	AcceptanceCalculator accCalc;
	accCalc.Init(th_y_sign, anal);

	// get input
	TChain *ch_in = new TChain("distilled");
	printf(">> input chain\n");
	for (const auto &ntupleDir : distilledNtuples)
	{
		string f = inputDir + "/" + ntupleDir + "/distill_" + argv[1] + ".root";
		printf("    %s\n", f.c_str());
		ch_in->Add(f.c_str());
	}
	printf("%llu entries\n", ch_in->GetEntries());

	// init output file
	TFile *outF = TFile::Open((outputDir+"/distributions_" + argv[1] + ".root").c_str(), "recreate");

	// get input data
	EventRed ev;
	ch_in->SetBranchAddress("v_L_1_F", &ev.h.L_1_F.v); ch_in->SetBranchAddress("x_L_1_F", &ev.h.L_1_F.x); ch_in->SetBranchAddress("y_L_1_F", &ev.h.L_1_F.y);
	ch_in->SetBranchAddress("v_L_2_F", &ev.h.L_2_F.v); ch_in->SetBranchAddress("x_L_2_F", &ev.h.L_2_F.x); ch_in->SetBranchAddress("y_L_2_F", &ev.h.L_2_F.y);

	ch_in->SetBranchAddress("v_R_1_F", &ev.h.R_1_F.v); ch_in->SetBranchAddress("x_R_1_F", &ev.h.R_1_F.x); ch_in->SetBranchAddress("y_R_1_F", &ev.h.R_1_F.y);
	ch_in->SetBranchAddress("v_R_2_F", &ev.h.R_2_F.v); ch_in->SetBranchAddress("x_R_2_F", &ev.h.R_2_F.x); ch_in->SetBranchAddress("y_R_2_F", &ev.h.R_2_F.y);	

	ch_in->SetBranchAddress("timestamp", &ev.timestamp);
	ch_in->SetBranchAddress("run_num", &ev.run_num);
	ch_in->SetBranchAddress("bunch_num", &ev.bunch_num);
	ch_in->SetBranchAddress("event_num", &ev.event_num);
	ch_in->SetBranchAddress("trigger_num", &ev.trigger_num);
	ch_in->SetBranchAddress("trigger_bits", &ev.trigger_bits);

	// get time-dependent pile-up correction
	TGraph *corrg_pileup = NULL;
	if (anal.use_pileup_efficiency_fits)
	{
		string path = inputDir + "/pileup_fit_combined.root";
		TFile *puF = TFile::Open(path.c_str());
		if (!puF)
			printf("ERROR: pile-up correction file `%s' cannot be opened.\n", path.c_str());
		if (diagonal == d45b_56t)
			corrg_pileup = (TGraph *) puF->Get("45b_56t/dgn");
		if (diagonal == d45t_56b)
			corrg_pileup = (TGraph *) puF->Get("45t_56b/dgn");
	}

	// get time-dependent resolution
	TGraph *g_d_x_RMS = NULL;
	TGraph *g_d_y_RMS = NULL;
	if (anal.use_resolution_fits)
	{
		string path = inputDir + "/resolution_fit_" + argv[1] + ".root";
		TFile *resFile = TFile::Open(path.c_str());
		if (!resFile)
			printf("ERROR: resolution file `%s' cannot be opened.\n", path.c_str());

		g_d_x_RMS = (TGraph *) resFile->Get("d_x/g_fits");
		g_d_y_RMS = (TGraph *) resFile->Get("d_y/g_fits");

		printf("\n>> using time-dependent resolutions: %p, %p\n", g_d_x_RMS, g_d_y_RMS);
	}

	// get th_y* dependent efficiency correction
	TF1 *f_3outof4_efficiency_L_2_F = NULL;
	TF1 *f_3outof4_efficiency_L_1_F = NULL;
	TF1 *f_3outof4_efficiency_R_1_F = NULL;
	TF1 *f_3outof4_efficiency_R_2_F = NULL;
	if (anal.use_3outof4_efficiency_fits)
	{
		string path = "/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/beta2500/4rp/efficiency_fits/global_fit.root";
		TFile *effFile = TFile::Open(path.c_str());
		if (!effFile)
			printf("ERROR: 3-out-of-4 efficiency file `%s' cannot be opened.\n", path.c_str());

		string diagonal = argv[1];
		f_3outof4_efficiency_L_2_F = (TF1 *) effFile->Get( (diagonal + "/L_2_F/pol1").c_str() );
		f_3outof4_efficiency_L_1_F = (TF1 *) effFile->Get( (diagonal + "/L_1_F/pol1").c_str() );
		f_3outof4_efficiency_R_1_F = (TF1 *) effFile->Get( (diagonal + "/R_1_F/pol1").c_str() );
		f_3outof4_efficiency_R_2_F = (TF1 *) effFile->Get( (diagonal + "/R_2_F/pol1").c_str() );

		printf("\n>> using 3-out-of-4 fits: %p, %p, %p, %p\n",
			f_3outof4_efficiency_L_2_F, f_3outof4_efficiency_L_1_F, f_3outof4_efficiency_R_1_F, f_3outof4_efficiency_R_2_F);

		if (!f_3outof4_efficiency_L_2_F || !f_3outof4_efficiency_L_1_F || !f_3outof4_efficiency_R_1_F || !f_3outof4_efficiency_R_2_F)
		{
			printf("ERROR: some of the 3-out-of-4 graphs can not be loaded.\n");
			return 100;
		}
	}

	// get unsmearing corrections
	bool apply_unsmearing = (unsmearing_file != "");

	printf("\n>> unsmearing_file = %s\n", unsmearing_file.c_str());
	printf(">> unsmearing_object = %s\n", unsmearing_object.c_str());
	printf(">> apply_unsmearing = %i\n", apply_unsmearing);

	map<unsigned int, TH1D*> unsmearing_corrections;

	if (apply_unsmearing)
	{
		string path = inputDir + "/" + unsmearing_file;
		TFile *unsmearing_correction_file = TFile::Open(path.c_str());
		if (!unsmearing_correction_file)
		{
			printf("ERROR: unfolding file `%s' can not be opened.\n", path.c_str());
			return 101;
		} else {
			for (unsigned int bi = 0; bi < binnings.size(); ++bi)
			{
				string obj_path = unsmearing_object;
				size_t pos = obj_path.find("<binning>");
				if (pos != string::npos)
				{
					obj_path.replace(pos, 9, binnings[bi]);
				}

				TH1D *h = (TH1D *) unsmearing_correction_file->Get(obj_path.c_str());
				if (h == NULL)
				{
					printf("ERROR: unsmearing correction object `%s' cannot be loaded.\n", obj_path.c_str());
					return 102;
				}

				unsmearing_corrections[bi] = h;
			}
		}
	}

	// book metadata histograms	
	unsigned int timestamp_bins = timestamp_max - timestamp_min + 1.;

	TH1D *h_timestamp_dgn = new TH1D("h_timestamp_dgn", ";timestamp;rate   (Hz)", timestamp_bins, timestamp_min-0.5, timestamp_max+0.5);
	TH1D *h_timestamp_B0 = new TH1D("h_timestamp_B0", ";timestamp;rate   (Hz)", timestamp_bins, timestamp_min-0.5, timestamp_max+0.5);
	TH1D *h_timestamp_sel = new TH1D("h_timestamp_sel", ";timestamp;rate   (Hz)", timestamp_bins, timestamp_min-0.5, timestamp_max+0.5);

	TH1D *h_trigger_bit = new TH1D("h_trigger_bit", ";trigger bit", 16, -0.5, 15.5);

	TGraph *g_run_vs_timestamp = new TGraph(); g_run_vs_timestamp->SetName("g_run_vs_timestamp"); g_run_vs_timestamp->SetTitle(";timestamp;run");
	TGraph *g_ev_num_vs_timestamp = new TGraph(); g_ev_num_vs_timestamp->SetName("g_ev_num_vs_timestamp"); g_ev_num_vs_timestamp->SetTitle(";timestamp;ev_num");
	TGraph *g_tr_num_vs_timestamp = new TGraph(); g_tr_num_vs_timestamp->SetName("g_tr_num_vs_timestamp"); g_tr_num_vs_timestamp->SetTitle(";timestamp;tr_num");
	TGraph *g_bunch_num_vs_timestamp = new TGraph(); g_bunch_num_vs_timestamp->SetName("g_bunch_num_vs_timestamp"); g_bunch_num_vs_timestamp->SetTitle(";timestamp;bunch");
	TGraph *g_selected_bunch_num_vs_timestamp = new TGraph(); g_selected_bunch_num_vs_timestamp->SetName("g_selected_bunch_num_vs_timestamp"); g_selected_bunch_num_vs_timestamp->SetTitle(";timestamp;selected_bunch");

	TGraph *g_timestamp_vs_ev_idx_dgn = new TGraph(); g_timestamp_vs_ev_idx_dgn->SetName("g_timestamp_vs_ev_idx_dgn"); g_timestamp_vs_ev_idx_dgn->SetTitle(";event index in distilled TTree;timestamp");
	TGraph *g_timestamp_vs_ev_idx_sel = new TGraph(); g_timestamp_vs_ev_idx_sel->SetName("g_timestamp_vs_ev_idx_sel"); g_timestamp_vs_ev_idx_sel->SetTitle(";event index in distilled TTree;timestamp");

	// book hit-distribution histograms
	TH2D *h_y_L_1_F_vs_x_L_1_F_al_nosel = new TH2D("h_y_L_1_F_vs_x_L_1_F_al_nosel", ";x^{L,1,F};y^{L,1,F}", 150, -15., 15., 300, -35., +35.);
	TH2D *h_y_L_2_F_vs_x_L_2_F_al_nosel = new TH2D("h_y_L_2_F_vs_x_L_2_F_al_nosel", ";x^{L,2,F};y^{L,2,F}", 150, -15., 15., 300, -35., +35.);
	TH2D *h_y_R_1_F_vs_x_R_1_F_al_nosel = new TH2D("h_y_R_1_F_vs_x_R_1_F_al_nosel", ";x^{R,1,F};y^{R,1,F}", 150, -15., 15., 300, -35., +35.);
	TH2D *h_y_R_2_F_vs_x_R_2_F_al_nosel = new TH2D("h_y_R_2_F_vs_x_R_2_F_al_nosel", ";x^{R,2,F};y^{R,2,F}", 150, -15., 15., 300, -35., +35.);

	TH2D *h_y_L_1_F_vs_x_L_1_F_noal_sel = new TH2D("h_y_L_1_F_vs_x_L_1_F_noal_sel", ";x^{L,1,F};y^{L,1,F}", 100, -15., +15., 300, -35., +35.);
	TH2D *h_y_L_2_F_vs_x_L_2_F_noal_sel = new TH2D("h_y_L_2_F_vs_x_L_2_F_noal_sel", ";x^{L,2,F};y^{L,2,F}", 100, -15., +15., 300, -35., +35.);
	TH2D *h_y_R_1_F_vs_x_R_1_F_noal_sel = new TH2D("h_y_R_1_F_vs_x_R_1_F_noal_sel", ";x^{R,1,F};y^{R,1,F}", 100, -15., +15., 300, -35., +35.);
	TH2D *h_y_R_2_F_vs_x_R_2_F_noal_sel = new TH2D("h_y_R_2_F_vs_x_R_2_F_noal_sel", ";x^{R,2,F};y^{R,2,F}", 100, -15., +15., 300, -35., +35.);

	TH2D *h_y_L_1_F_vs_x_L_1_F_al_sel = new TH2D("h_y_L_1_F_vs_x_L_1_F_al_sel", ";x^{L,1,F};y^{L,1,F}", 100, -15., +15., 300, -35., +35.);
	TH2D *h_y_L_2_F_vs_x_L_2_F_al_sel = new TH2D("h_y_L_2_F_vs_x_L_2_F_al_sel", ";x^{L,2,F};y^{L,2,F}", 100, -15., +15., 300, -35., +35.);
	TH2D *h_y_R_1_F_vs_x_R_1_F_al_sel = new TH2D("h_y_R_1_F_vs_x_R_1_F_al_sel", ";x^{R,1,F};y^{R,1,F}", 100, -15., +15., 300, -35., +35.);
	TH2D *h_y_R_2_F_vs_x_R_2_F_al_sel = new TH2D("h_y_R_2_F_vs_x_R_2_F_al_sel", ";x^{R,2,F};y^{R,2,F}", 100, -15., +15., 300, -35., +35.);

	/*
	TGraph *g_y_L_1_F_vs_x_L_1_F_al_sel = new TGraph(); g_y_L_1_F_vs_x_L_1_F_al_sel->SetName("g_y_L_1_F_vs_x_L_1_F_al_sel");
	TGraph *g_y_L_2_F_vs_x_L_2_F_al_sel = new TGraph(); g_y_L_2_F_vs_x_L_2_F_al_sel->SetName("g_y_L_2_F_vs_x_L_2_F_al_sel");
	TGraph *g_y_R_1_F_vs_x_R_1_F_al_sel = new TGraph(); g_y_R_1_F_vs_x_R_1_F_al_sel->SetName("g_y_R_1_F_vs_x_R_1_F_al_sel");
	TGraph *g_y_R_2_F_vs_x_R_2_F_al_sel = new TGraph(); g_y_R_2_F_vs_x_R_2_F_al_sel->SetName("g_y_R_2_F_vs_x_R_2_F_al_sel");
	*/

	// book alignment histograms
	map<signed int, TGraph *> g_y_L_1_F_vs_x_L_1_F_sel, g_y_L_2_F_vs_x_L_2_F_sel;
	map<signed int, TGraph *> g_y_R_1_F_vs_x_R_1_F_sel, g_y_R_2_F_vs_x_R_2_F_sel;

	map<signed int, TGraph *> g_w_vs_timestamp_sel;

	map<signed int, TH1D *> tm_h_th_x_L, tm_h_th_x_R;
	map<signed int, TProfile *> tm_p_diffLR_th_x, tm_p_diffLR_th_y;

	// book cut histograms
	map<unsigned int, TH1D *> h_cq;
	map<unsigned int, TH2D *> h2_cq, h2_cq_full;
	//map<unsigned int, TGraph *> g_cq;
	map<unsigned int, TProfile *> p_cq, p_cq_time;
	for (unsigned int i = 1; i <= anal.N_cuts; ++i)
	{
		char name[100], title[100];

		double x_min=0., x_max=0., y_min=0., y_max = 0.;
		double q_max = 0.;

		if (i == 1) { x_min = -200E-6; x_max = +200E-6; y_min = -200E-6; y_max = 200E-6; q_max = 80E-6; }
		if (i == 2) { x_min = -150E-6; x_max = +150E-6; y_min = -150E-6; y_max = 150E-6; q_max = 15E-6; }
		if (i == 3) { x_min = -1000E-6; x_max = +1000E-6; y_min = -1.5; y_max = 1.5; }
		if (i == 4) { x_min = -1000E-6; x_max = +1000E-6; y_min = -1.5; y_max = 1.5; }
		if (i == 5) { x_min = -30.; x_max = +30.; y_min = -5.; y_max = 5.; q_max = 150E-3; }
		if (i == 6) { x_min = -30.; x_max = +30.; y_min = -5.; y_max = 5.; q_max = 150E-3; }
		if (i == 7) { x_min = -200E-6; x_max = +200E-6; y_min = -10.; y_max = +10.; q_max = 10.; }
		if (i == 8) { x_min = -600E-6; x_max = +600E-6; y_min = -4.; y_max = +4.; q_max = 500E-3; }

		if (i == 9) { x_min = -200E-6; x_max = +200E-6; y_min = -200E-6; y_max = 200E-6; q_max = 80E-6; }

		sprintf(name, "h_cq%i", i); sprintf(title, ";cq%i", i); h_cq[i] = new TH1D(name, title, 200, -q_max, +q_max);

		sprintf(name, "h2_cq%i", i); sprintf(title, ";%s;%s", anal.cqaN[i].c_str(), anal.cqbN[i].c_str()); h2_cq[i] = new TH2D(name, title, 100, x_min, x_max, 100, y_min, y_max);
		sprintf(name, "h2_cq_full%i", i); sprintf(title, ";%s;%s", anal.cqaN[i].c_str(), anal.cqbN[i].c_str()); h2_cq_full[i] = new TH2D(name, title, 100, x_min, x_max, 100, y_min, y_max);

		//sprintf(name, "g_cq%i", i); sprintf(title, ";%s;%s", anal.cqaN[i].c_str(), anal.cqbN[i].c_str()); g_cq[i] = new TGraph(); g_cq[i]->SetName(name); g_cq[i]->SetTitle(title);
		sprintf(name, "p_cq%i", i); sprintf(title, ";%s;%s", anal.cqaN[i].c_str(), anal.cqbN[i].c_str()); p_cq[i] = new TProfile(name, title, 300, 0., 0.);

		int timestamp_bins = (timestamp_max - timestamp_min) / 300;
		sprintf(name, "p_cq_time%i", i); sprintf(title, ";time   (s);mean of cq%i", i); p_cq_time[i] = new TProfile(name, title, timestamp_bins, timestamp_min, timestamp_max);
	}

	// book histograms for selected hits
	/*
	TProfile *p_x_vs_y_L_F = new TProfile("p_x_vs_y_L_F", ";y^{L,F};x^{L,F};", 50, 0., 0.);
	TProfile *p_x_vs_y_L_N = new TProfile("p_x_vs_y_L_N", ";y^{L,N};x^{L,N};", 50, 0., 0.);
	TProfile *p_x_vs_y_R_N = new TProfile("p_x_vs_y_R_N", ";y^{R,N};x^{R,N};", 50, 0., 0.);
	TProfile *p_x_vs_y_R_F = new TProfile("p_x_vs_y_R_F", ";y^{R,F};x^{R,F};", 50, 0., 0.);

	TProfile *p_x_vs_y_L_F_noal = new TProfile("p_x_vs_y_L_F_noal", ";y^{L,F};x^{L,F};", 50, 0., 0.);
	TProfile *p_x_vs_y_L_N_noal = new TProfile("p_x_vs_y_L_N_noal", ";y^{L,N};x^{L,N};", 50, 0., 0.);
	TProfile *p_x_vs_y_R_N_noal = new TProfile("p_x_vs_y_R_N_noal", ";y^{R,N};x^{R,N};", 50, 0., 0.);
	TProfile *p_x_vs_y_R_F_noal = new TProfile("p_x_vs_y_R_F_noal", ";y^{R,F};x^{R,F};", 50, 0., 0.);

	TH2D *h_y_L_diffFN_vs_y_L_N = new TH2D("h_y_L_diffFN_vs_y_L_N", ";y^{LN};y^{LF} - y^{LN}", 300, -30., +30., 300, -3., +3.);
	TH2D *h_y_R_diffFN_vs_y_R_N = new TH2D("h_y_R_diffFN_vs_y_R_N", ";y^{RN};y^{RF} - y^{RN}", 300, -30., +30., 300, -3., +3.);

	TH2D *h_y_L_ratioFN_vs_y_L_N = new TH2D("h_y_L_ratioFN_vs_y_L_N", ";y^{LN};y^{LF} / y^{LN}", 300, -30., +30., 300, 1.08, 1.14);
	TH2D *h_y_R_ratioFN_vs_y_R_N = new TH2D("h_y_R_ratioFN_vs_y_R_N", ";y^{RN};y^{RF} / y^{RN}", 300, -30., +30., 300, 1.08, 1.14);
	*/

	// book angluar histograms
	TH1D *th_x_diffLR = new TH1D("th_x_diffLR", ";#theta_{x}^{R} - #theta_{x}^{L}", 1000, -500E-6, +500E-6); th_x_diffLR->Sumw2();
	TH1D *th_y_diffLR = new TH1D("th_y_diffLR", ";#theta_{y}^{R} - #theta_{y}^{L}", 500, -50E-6, +50E-6); th_y_diffLR->Sumw2();

	TH1D *th_x_diffLF = new TH1D("th_x_diffLF", ";#theta_{x}^{L} - #theta_{x}", 400, -200E-6, +200E-6); th_x_diffLF->Sumw2();
	TH1D *th_x_diffRF = new TH1D("th_x_diffRF", ";#theta_{x}^{R} - #theta_{x}", 400, -200E-6, +200E-6); th_x_diffRF->Sumw2();

	TH2D *h_th_x_diffLR_vs_th_x = new TH2D("h_th_x_diffLR_vs_th_x", ";#theta_{x};#theta_{x}^{R} - #theta_{x}^{L}", 100, -300E-6, +300E-6, 120, -120E-6, +120E-6);
	TH2D *h_th_y_diffLR_vs_th_y = new TH2D("h_th_y_diffLR_vs_th_y", ";#theta_{y};#theta_{y}^{R} - #theta_{y}^{L}", 100, -500E-6, +500E-6, 120, -120E-6, +120E-6);

	TH2D *h_th_x_diffLR_vs_vtx_x = new TH2D("h_th_x_diffLR_vs_vtx_x", ";vtx_{x};#theta_{x}^{R} - #theta_{x}^{L}", 100, -300E-3, +300E-3, 120, -120E-6, +120E-6);

	TProfile *p_th_x_diffLR_vs_th_x = new TProfile("p_th_x_diffLR_vs_th_x", ";#theta_{x};#theta_{x}^{R} - #theta_{x}^{L}", 100, -100E-6, +100E-6);
	TProfile *p_th_x_L_diff12_vs_th_x_L = new TProfile("p_th_x_L_diff12_vs_th_x_L", ";#theta_{x}^{L};#theta_{x}^{L2F} - #theta_{x}^{L1F}", 100, -500E-6, +500E-6);
	TProfile *p_th_x_R_diff12_vs_th_x_R = new TProfile("p_th_x_R_diff12_vs_th_x_R", ";#theta_{x}^{R};#theta_{x}^{R2F} - #theta_{x}^{R1F}", 100, -500E-6, +500E-6);

	TProfile *p_th_y_diffLR_vs_th_y = new TProfile("p_th_y_diffLR_vs_th_y", ";#theta_{y};#theta_{y}^{R} - #theta_{y}^{L}", 200, -100E-6, +100E-6);
	TProfile *p_th_y_L_diff12_vs_th_y_L = new TProfile("p_th_y_L_diff12_vs_th_y_L", ";#theta_{y}^{L};#theta_{y}^{L2F} - #theta_{y}^{L1F}", 200, -500E-6, +500E-6);
	TProfile *p_th_y_R_diff12_vs_th_y_R = new TProfile("p_th_y_R_diff12_vs_th_y_R", ";#theta_{y}^{R};#theta_{y}^{R2F} - #theta_{y}^{R1F}", 200, -500E-6, +500E-6);

	TProfile *p_th_x_diffLR_vs_vtx_x = new TProfile("p_th_x_diffLR_vs_vtx_x", ";vtx_{x};#theta_{x}^{R} - #theta_{x}^{L}", 200, -400E-3, +400E-3);

	TH1D *th_x_diffLR_safe = new TH1D("th_x_diffLR_safe", ";#theta_{x}^{R} - #theta_{x}^{L}", 100, 0., 0.); th_x_diffLR_safe->Sumw2();
	TH1D *th_y_diffLR_safe = new TH1D("th_y_diffLR_safe", ";#theta_{y}^{R} - #theta_{y}^{L}", 100, 0., 0.); th_y_diffLR_safe->Sumw2();

	TProfile *p_th_x_vs_th_y = new TProfile("p_th_x_vs_th_y", ";#theta_{y}^{R};#theta_{x}", 100, -500E-6, +500E-6);
	TProfile *p_th_x_L_vs_th_y_L = new TProfile("p_th_x_L_vs_th_y_L", ";#theta_{y}^{L};#theta_{x}^{L}", 100, -500E-6, +500E-6);
	TProfile *p_th_x_R_vs_th_y_R = new TProfile("p_th_x_R_vs_th_y_R", ";#theta_{y}^{R};#theta_{x}^{R}", 100, -500E-6, +500E-6);

	TH2D *h_th_y_L_vs_th_x_L = new TH2D("h_th_y_L_vs_th_x_L", ";#theta_{x}^{L};#theta_{y}^{L}", 160, -200E-6, +200E-6, 960, -120E-6, +120E-6);
	TH2D *h_th_y_R_vs_th_x_R = new TH2D("h_th_y_R_vs_th_x_R", ";#theta_{x}^{R};#theta_{y}^{R}", 160, -200E-6, +200E-6, 960, -120E-6, +120E-6);
	TH2D *h_th_y_vs_th_x = new TH2D("h_th_y_vs_th_x", ";#theta_{x};#theta_{y}", 160, -200E-6, +200E-6, 960, -120E-6, +120E-6);

	TGraph *g_th_y_L_vs_th_x_L = new TGraph(); g_th_y_L_vs_th_x_L->SetName("g_th_y_L_vs_th_x_L"); g_th_y_L_vs_th_x_L->SetTitle(";#theta_{x}^{L};#theta_{y}^{L}");
	TGraph *g_th_y_R_vs_th_x_R = new TGraph(); g_th_y_R_vs_th_x_R->SetName("g_th_y_R_vs_th_x_R"); g_th_y_R_vs_th_x_R->SetTitle(";#theta_{x}^{R};#theta_{y}^{R}");
	TGraph *g_th_y_vs_th_x = new TGraph(); g_th_y_vs_th_x->SetName("g_th_y_vs_th_x"); g_th_y_vs_th_x->SetTitle(";#theta_{x}^{L};#theta_{y}^{L}");

	TH2D *h_th_y_L_vs_th_y_R = new TH2D("h_th_y_L_vs_th_y_R", ";#theta_{y}^{R};#theta_{y}^{L}", 300, -150E-6, +150E-6, 300, -150E-6, +150E-6);
	TGraph *g_th_y_L_vs_th_y_R = new TGraph(); g_th_y_L_vs_th_y_R->SetName("g_th_y_L_vs_th_y_R"); g_th_y_L_vs_th_y_R->SetTitle(";#theta_{y}^{R};#theta_{y}^{L}");

	TH1D *h_th_x = new TH1D("h_th_x", ";#theta_{x}", 250, -500E-6, +500E-6); h_th_x->SetLineColor(1);
	TH1D *h_th_y = new TH1D("h_th_y", ";#theta_{y}", 250, -500E-6, +500E-6); h_th_y->SetLineColor(1);
	TH1D *h_th_y_flipped = new TH1D("h_th_y_flipped", ";#theta_{y}", 250, -500E-6, +500E-6); h_th_y_flipped->SetLineColor(1);

	TH1D *h_th_x_L = new TH1D("h_th_x_L", ";#theta_{x}^{L}", 250, -500E-6, +500E-6); h_th_x_L->SetLineColor(2);
	TH1D *h_th_x_R = new TH1D("h_th_x_R", ";#theta_{x}^{R}", 250, -500E-6, +500E-6); h_th_x_R->SetLineColor(4);

	TH1D *h_th_y_L = new TH1D("h_th_y_L", ";#theta_{y}^{L}", 250, -500E-6, +500E-6); h_th_y_L->SetLineColor(2);
	TH1D *h_th_y_R = new TH1D("h_th_y_R", ";#theta_{y}^{R}", 250, -500E-6, +500E-6); h_th_y_R->SetLineColor(4);

	TH1D *h_th_y_L_2_F = new TH1D("h_th_y_L_2_F", ";#theta_{y}^{L_2_F}", 250, -500E-6, +500E-6); h_th_y_L_2_F->SetLineColor(2);
	TH1D *h_th_y_L_1_F = new TH1D("h_th_y_L_1_F", ";#theta_{y}^{L_1_F}", 250, -500E-6, +500E-6); h_th_y_L_1_F->SetLineColor(6);
	TH1D *h_th_y_R_1_F = new TH1D("h_th_y_R_1_F", ";#theta_{y}^{R_1_F}", 250, -500E-6, +500E-6); h_th_y_R_1_F->SetLineColor(4);
	TH1D *h_th_y_R_2_F = new TH1D("h_th_y_R_2_F", ";#theta_{y}^{R_2_F}", 250, -500E-6, +500E-6); h_th_y_R_2_F->SetLineColor(7);

	// book alternative angular histograms
	/*
	TH1D *h_ta_th_x_R = new TH1D("h_ta_th_x_R", ";#tau_{x}^{R} - #theta_{x}^{R}", 100, 0., 0.);
	TH1D *h_ta_th_x_L = new TH1D("h_ta_th_x_L", ";#tau_{x}^{L} - #theta_{x}^{L}", 100, 0., 0.);
	TH1D *h_ta_th_x = new TH1D("h_ta_th_x", ";#tau_{x} - #theta_{x}", 100, 0., 0.);
	TH1D *h_ta_x_diffLR = new TH1D("h_ta_x_diffLR", ";#Delta^{R-L} #tau_{x}", 100, 0., 0.);

	TH1D *h_ta_y_R = new TH1D("h_ta_y_R", ";#tau_{y}^{R}", 100, -600E-6, +600E-6);
	TH1D *h_ta_y_L = new TH1D("h_ta_y_L", ";#tau_{y}^{L}", 100, -600E-6, +600E-6);

	TH1D *h_ta_th_y = new TH1D("h_ta_th_y", ";#tau_{y} - #theta_{y}", 100, 0., 0.);
	TH1D *h_ta_th_y_R = new TH1D("h_ta_th_y_R", ";#tau_{y}^{R} - #theta_{y}^{R}", 100, 0., 0.);
	TH1D *h_ta_th_y_L = new TH1D("h_ta_th_y_L", ";#tau_{y}^{L} - #theta_{y}^{L}", 100, 0., 0.);
	TH1D *h_ta_y_diffLR = new TH1D("h_ta_y_diffLR", ";#Delta^{R-L} #tau_{y}", 100, 0., 0.);

	TProfile *h_ta_y_vs_th_y = new TProfile("h_ta_y_vs_th_y", ";#theta_{y};#tau_{y}", 100, -600E-6, +600E-6); h_ta_y_vs_th_y->SetLineColor(1);
	TProfile *h_ta_y_L_vs_th_y_L = new TProfile("h_ta_y_L_vs_th_y_L", ";#theta_{y}^{L};#tau_{y}^{L}", 100, -600E-6, +600E-6); h_ta_y_L_vs_th_y_L->SetLineColor(2);
	TProfile *h_ta_y_R_vs_th_y_R = new TProfile("h_ta_y_R_vs_th_y_R", ";#theta_{y}^{R};#tau_{y}^{R}", 100, -600E-6, +600E-6); h_ta_y_R_vs_th_y_R->SetLineColor(4);
	*/

	// vertex histograms
	TH1D *h_vtx_x = new TH1D("h_vtx_x", ";x^{*}", 100, -0.5, +0.5); h_vtx_x->SetLineColor(1);
	TH1D *h_vtx_x_L = new TH1D("h_vtx_x_L", ";x^{*,L}", 100, -0.5, +0.5); h_vtx_x_L->SetLineColor(2);
	TH1D *h_vtx_x_R = new TH1D("h_vtx_x_R", ";x^{*,R}", 100, -0.5, +0.5); h_vtx_x_R->SetLineColor(4);

	TH1D *h_vtx_y = new TH1D("h_vtx_y", ";y^{*}", 100, -0.5, +0.5); h_vtx_y->SetLineColor(1);
	TH1D *h_vtx_y_L = new TH1D("h_vtx_y_L", ";y^{*,L}", 100, -0.5, +0.5); h_vtx_y_L->SetLineColor(2);
	TH1D *h_vtx_y_R = new TH1D("h_vtx_y_R", ";y^{*,R}", 100, -0.5, +0.5); h_vtx_y_R->SetLineColor(4);

	TH1D *h_vtx_x_safe = new TH1D("h_vtx_x_safe", ";x^{*}", 100, -0.5, +0.5); h_vtx_x_safe->SetLineColor(6);
	TH1D *h_vtx_y_safe = new TH1D("h_vtx_y_safe", ";y^{*}", 100, -0.5, +0.5); h_vtx_y_safe->SetLineColor(6);

	TH2D *h_vtx_x_L_vs_vtx_x_R = new TH2D("h_vtx_x_L_vs_vtx_x_R", ";x^{*,R};x^{*,L}", 100, -0.5, +0.5, 100, -0.5, +0.5);
	TH2D *h_vtx_y_L_vs_vtx_y_R = new TH2D("h_vtx_y_L_vs_vtx_y_R", ";y^{*,R};y^{*,L}", 100, -0.5, +0.5, 100, -0.5, +0.5);

	TH2D *h_vtx_x_L_vs_th_x_L = new TH2D("h_vtx_x_L_vs_th_x_L", ";#theta_{x}^{L};x^{*,L}", 100, -600E-6, +600E-6, 100, -0.5, +0.5);
	TH2D *h_vtx_x_R_vs_th_x_R = new TH2D("h_vtx_x_R_vs_th_x_R", ";#theta_{x}^{R};x^{*,R}", 100, -600E-6, +600E-6, 100, -0.5, +0.5);
	TH2D *h_vtx_y_L_vs_th_y_L = new TH2D("h_vtx_y_L_vs_th_y_L", ";#theta_{y}^{L};y^{*,L}", 100, -600E-6, +600E-6, 100, -0.5, +0.5);
	TH2D *h_vtx_y_R_vs_th_y_R = new TH2D("h_vtx_y_R_vs_th_y_R", ";#theta_{y}^{R};y^{*,R}", 100, -600E-6, +600E-6, 100, -0.5, +0.5);

	TH1D *h_vtx_x_diffLR = new TH1D("h_vtx_x_diffLR", ";x^{*,R} - x^{*,L}", 100, -0.5, +0.5); h_vtx_x_diffLR->Sumw2();
	TH1D *h_vtx_y_diffLR = new TH1D("h_vtx_y_diffLR", ";y^{*,R} - y^{*,L}", 100, -0.5, +0.5); h_vtx_y_diffLR->Sumw2();

	TH1D *h_vtx_x_diffLR_safe = new TH1D("h_vtx_x_diffLR_safe", ";vtx_{x}^{R} - vtx_{x}^{L}", 100, -0.5, +0.5); h_vtx_x_diffLR_safe->Sumw2(); h_vtx_x_diffLR_safe->SetLineColor(6);
	TH1D *h_vtx_y_diffLR_safe = new TH1D("h_vtx_y_diffLR_safe", ";vtx_{y}^{R} - vtx_{y}^{L}", 100, -0.5, +0.5); h_vtx_y_diffLR_safe->Sumw2(); h_vtx_y_diffLR_safe->SetLineColor(6);

	TH1D *h_vtx_x_diffLR_safe_corr = new TH1D("h_vtx_x_diffLR_safe_corr", ";vtx_{x}^{R} - vtx_{x}^{L}", 100, -0.5, +0.5); h_vtx_x_diffLR_safe_corr->Sumw2(); h_vtx_x_diffLR_safe_corr->SetLineColor(6);
	TH1D *h_vtx_y_diffLR_safe_corr = new TH1D("h_vtx_y_diffLR_safe_corr", ";vtx_{y}^{R} - vtx_{y}^{L}", 100, -0.5, +0.5); h_vtx_y_diffLR_safe_corr->Sumw2(); h_vtx_y_diffLR_safe_corr->SetLineColor(6);

	TH2D *h_vtx_x_diffLR_vs_th_x = new TH2D("h_vtx_x_diffLR_vs_th_x", ";#theta_{x};x^{*,R} - x^{*,L}", 100, -600E-6, +600E-6, 100, -0.5, +0.5);
	TH2D *h_vtx_y_diffLR_vs_th_y = new TH2D("h_vtx_y_diffLR_vs_th_y", ";#theta_{y};y^{*,R} - y^{*,L}", 200, -600E-6, +600E-6, 100, -0.5, +0.5);

	TProfile *p_vtx_x_diffLR_vs_th_x = new TProfile("p_vtx_x_diffLR_vs_th_x", ";#theta_{x};x^{*,R} - x^{*,L}", 100, -600E-6, +600E-6);
	TProfile *p_vtx_y_diffLR_vs_th_y = new TProfile("p_vtx_y_diffLR_vs_th_y", ";#theta_{y};y^{*,R} - y^{*,L}", 200, -600E-6, +600E-6);

	TH2D *h_vtx_x_diffLR_vs_vtx_x_R = new TH2D("h_vtx_x_diffLR_vs_vtx_x_R", ";x^{*,R};x^{*,R} - x^{*,L}", 100, -0.5, +0.5, 100, -0.5, +0.5);
	TH2D *h_vtx_y_diffLR_vs_vtx_y_R = new TH2D("h_vtx_y_diffLR_vs_vtx_y_R", ";y^{*,R};y^{*,R} - y^{*,L}", 100, -0.5, +0.5, 100, -0.5, +0.5);

	// input for optics matching
	OpticsMatchingInput opticsMatchingIntput_full, opticsMatchingIntput_vtx_x_0p3sigma, opticsMatchingIntput_vtx_x_0p1sigma;

	// time-dependence histograms
	double period_len = 5*60.;	// s
	signed int period_min = floor(timestamp_min / period_len);
	signed int period_max = floor(timestamp_max / period_len);
	unsigned int time_bins = period_max - period_min + 1;
	double time_min = period_min * period_len;
	double time_max = (period_max+1) * period_len;

	TProfile *p_th_x_L1F_vs_time = new TProfile("p_th_x_L1F_vs_time", ";timestamp;#theta_{x}^{L1F}", time_bins, time_min, time_max);
	TProfile *p_th_x_L2F_vs_time = new TProfile("p_th_x_L2F_vs_time", ";timestamp;#theta_{x}^{L2F}", time_bins, time_min, time_max);
	TProfile *p_th_x_L_vs_time = new TProfile("p_th_x_L_vs_time", ";timestamp;#theta_{x}^{L}", time_bins, time_min, time_max);
	TProfile *p_th_x_R1F_vs_time = new TProfile("p_th_x_R1F_vs_time", ";timestamp;#theta_{x}^{R1F}", time_bins, time_min, time_max);
	TProfile *p_th_x_R2F_vs_time = new TProfile("p_th_x_R2F_vs_time", ";timestamp;#theta_{x}^{R2F}", time_bins, time_min, time_max);
	TProfile *p_th_x_R_vs_time = new TProfile("p_th_x_R_vs_time", ";timestamp;#theta_{x}^{R}", time_bins, time_min, time_max);
	TProfile *p_th_x_vs_time = new TProfile("p_th_x_vs_time", ";timestamp;#theta_{x}", time_bins, time_min, time_max);

	TProfile *p_th_y_L_vs_time = new TProfile("p_th_y_L_vs_time", ";timestamp;#theta_{y}^{L}", time_bins, time_min, time_max);
	TProfile *p_th_y_R_vs_time = new TProfile("p_th_y_R_vs_time", ";timestamp;#theta_{y}^{R}", time_bins, time_min, time_max);
	TProfile *p_th_y_vs_time = new TProfile("p_th_y_vs_time", ";timestamp;#theta_{y}", time_bins, time_min, time_max);

	TProfile *p_diffLR_th_x_vs_time = new TProfile("p_diffLR_th_x_vs_time", ";timestamp;mean of #Delta^{R-L}#theta_{x}", time_bins, time_min, time_max);
	TProfile *p_diffLR_th_y_vs_time = new TProfile("p_diffLR_th_y_vs_time", ";timestamp;mean of #Delta^{R-L}#theta_{y}", time_bins, time_min, time_max);

	TProfile *p_kin_cuts_th_x_L_vs_time = new TProfile("p_kin_cuts_th_x_L_vs_time", ";timestamp;#theta_{x}^{L}", time_bins, time_min, time_max);
	TProfile *p_kin_cuts_th_x_R_vs_time = new TProfile("p_kin_cuts_th_x_R_vs_time", ";timestamp;#theta_{x}^{R}", time_bins, time_min, time_max);
	TProfile *p_kin_cuts_th_x_vs_time = new TProfile("p_kin_cuts_th_x_vs_time", ";timestamp;#theta_{x}", time_bins, time_min, time_max);

	TProfile *p_kin_cuts_th_y_L_vs_time = new TProfile("p_kin_cuts_th_y_L_vs_time", ";timestamp;#theta_{y}^{L}", time_bins, time_min, time_max);
	TProfile *p_kin_cuts_th_y_R_vs_time = new TProfile("p_kin_cuts_th_y_R_vs_time", ";timestamp;#theta_{y}^{R}", time_bins, time_min, time_max);
	TProfile *p_kin_cuts_th_y_vs_time = new TProfile("p_kin_cuts_th_y_vs_time", ";timestamp;#theta_{y}", time_bins, time_min, time_max);

	TProfile *p_kin_cuts_vtx_x_vs_time = new TProfile("p_kin_cuts_vtx_x_vs_time", ";timestamp;mean of x^{*}", time_bins, time_min, time_max);

	TProfile *p_kin_cuts_diffLR_th_x_vs_time = new TProfile("p_kin_cuts_diffLR_th_x_vs_time", ";timestamp;mean of #Delta^{R-L}#theta_{x}", time_bins, time_min, time_max);
	TProfile *p_kin_cuts_diffLR_th_y_vs_time = new TProfile("p_kin_cuts_diffLR_th_y_vs_time", ";timestamp;mean of #Delta^{R-L}#theta_{y}", time_bins, time_min, time_max);
	TProfile *p_kin_cuts_diff12_th_y_L_vs_time = new TProfile("p_kin_cuts_diff12_th_y_L_vs_time", ";timestamp;mean of #Delta^{F-N}#theta_{y}^{L}", time_bins, time_min, time_max);
	TProfile *p_kin_cuts_diff12_th_y_R_vs_time = new TProfile("p_kin_cuts_diff12_th_y_R_vs_time", ";timestamp;mean of #Delta^{F-N}#theta_{y}^{R}", time_bins, time_min, time_max);

	TProfile *p_input_d_x_rms_vs_time = new TProfile("p_input_d_x_rms_vs_time", ";timestamp", time_bins, time_min, time_max);
	TProfile *p_input_d_y_rms_vs_time = new TProfile("p_input_d_y_rms_vs_time", ";timestamp", time_bins, time_min, time_max);

	// book acceptance-correction histograms
	TProfile *p_t_ub_div_corr = new TProfile("p_t_ub_div_corr", ";t_ub_{y}", 2000., 0., 0.2);

	map<unsigned int, TH1D*> bh_t_Nev_before, bh_t_Nev_after_no_corr;
	map<unsigned int, TH1D*> bh_t_before, bh_t_after_no_corr, bh_t_after;
	map<unsigned int, TProfile*> bp_t_phi_corr, bp_t_full_corr;

	for (unsigned int bi = 0; bi < binnings.size(); ++bi)
	{
		unsigned int N_bins;
		double *bin_edges;
		BuildBinning(anal, binnings[bi], bin_edges, N_bins);

		bh_t_Nev_before[bi] = new TH1D("h_t_Nev_before", ";|t|;events per bin", N_bins, bin_edges); bh_t_Nev_before[bi]->Sumw2();
		bh_t_Nev_after_no_corr[bi] = new TH1D("h_t_Nev_after_no_corr", ";|t|;events per bin", N_bins, bin_edges); bh_t_Nev_after_no_corr[bi]->Sumw2();
		bh_t_before[bi] = new TH1D("h_t_before", ";|t|", N_bins, bin_edges); bh_t_before[bi]->Sumw2();
		bh_t_after[bi] = new TH1D("h_t_after", ";|t|", N_bins, bin_edges); bh_t_after[bi]->Sumw2();
		bh_t_after_no_corr[bi] = new TH1D("h_t_after_no_corr", ";|t|", N_bins, bin_edges); bh_t_after_no_corr[bi]->Sumw2();
		bp_t_phi_corr[bi] = new TProfile("p_t_phi_corr", ";t", N_bins, bin_edges, "s");
		bp_t_full_corr[bi] = new TProfile("p_t_full_corr", ";t", N_bins, bin_edges, "s");
	}

	TH2D *h_th_y_vs_th_x_before = new TH2D("h_th_y_vs_th_x_before", ";#theta_{x};#theta_{y}", 150, -300E-6, +300E-6, 150, -150E-6, +150E-6); h_th_y_vs_th_x_before->Sumw2();
	TH2D *h_th_y_vs_th_x_after = new TH2D("h_th_y_vs_th_x_after", ";#theta_{x};#theta_{y}", 150, -300E-6, +300E-6, 150, -150E-6, +150E-6); h_th_y_vs_th_x_after->Sumw2();
	TH2D *h_th_vs_phi_after = new TH2D("h_th_vs_phi_after", ";#phi;#theta", 50, -M_PI, +M_PI, 50, 150E-6, 550E-6); h_th_vs_phi_after->Sumw2();

	TGraph *g_weight_vs_th_y = new TGraph(); g_weight_vs_th_y->SetName("g_weight_vs_th_y"); g_weight_vs_th_y->SetTitle(";#theta_{y};weight");

	TGraph *g_th_y_vs_th_x_acc = new TGraph(); g_th_y_vs_th_x_acc->SetName("g_th_y_vs_th_x_acc"); g_th_y_vs_th_x_acc->SetTitle(";#theta_{x}^{L};#theta_{y}^{L}");

	// book normalization histograms
	TProfile *p_norm_corr = new TProfile("p_norm_corr", ";timestamp", 1000, timestamp_min, timestamp_max);
	TProfile *p_3outof4_corr = new TProfile("p_3outof4_corr", ";#theta_{y}^{*}", 120, -120E-6, 120E-6);

	map<unsigned int, TH1D*> bh_t_normalized;
	for (unsigned int bi = 0; bi < binnings.size(); ++bi)
	{
		bh_t_normalized[bi] = new TH1D(* bh_t_after[bi]);
		bh_t_normalized[bi]->SetName("h_t_normalized");
	}

	TH2D *h_th_y_vs_th_x_normalized = new TH2D("h_th_y_vs_th_x_normalized", ";#theta_{x};#theta_{y}", 150, -600E-6, +600E-6, 150, -600E-6, +600E-6); h_th_y_vs_th_x_normalized->Sumw2();

	TGraph *g_norm_corr_vs_div_corr = new TGraph(); g_norm_corr_vs_div_corr->SetName("g_norm_corr_vs_div_corr"); g_norm_corr_vs_div_corr->SetTitle(";div_corr;norm_corr");

	// book histograms for optics matching
	/*
	TGraph *g_th_x_L_vs_th_x_R = new TGraph(); g_th_x_L_vs_th_x_R->SetName("g_th_x_L_vs_th_x_R"); g_th_x_L_vs_th_x_R->SetTitle(";#theta_{x}^{R};#theta_{x}^{L}");
	TGraph *g_th_x_R_vs_th_x_L = new TGraph(); g_th_x_R_vs_th_x_L->SetName("g_th_x_R_vs_th_x_L"); g_th_x_R_vs_th_x_L->SetTitle(";#theta_{x}^{L};#theta_{x}^{R}");

	TGraph *g_th_y_L_vs_th_y_R = new TGraph(); g_th_y_L_vs_th_y_R->SetName("g_th_y_L_vs_th_y_R"); g_th_y_L_vs_th_y_R->SetTitle(";#theta_{y}^{R};#theta_{y}^{L}");
	TGraph *g_th_y_R_vs_th_y_L = new TGraph(); g_th_y_R_vs_th_y_L->SetName("g_th_y_R_vs_th_y_L"); g_th_y_R_vs_th_y_L->SetTitle(";#theta_{y}^{L};#theta_{y}^{R}");

	TProfile *p_th_x_L_vs_th_x_R = new TProfile("p_th_x_L_vs_th_x_R", ";#theta_{x}^{R};#theta_{x}^{L}", 100, 0., 0.);
	TProfile *p_th_y_L_vs_th_y_R = new TProfile("p_th_y_L_vs_th_y_R", ";#theta_{y}^{R};#theta_{y}^{L}", 100, 150E-6, 550E-6);

	TProfile *p_x_N_L_vs_x_N_R = new TProfile("p_x_N_L_vs_x_N_R", ";x^{N,R};x^{N,L}", 100, 0., 0.);
	TProfile *p_x_F_L_vs_x_F_R = new TProfile("p_x_F_L_vs_x_F_R", ";x^{F,R};x^{F,L}", 100, 0., 0.);
	TProfile *p_y_N_L_vs_y_N_R = new TProfile("p_y_N_L_vs_y_N_R", ";y^{N,R};y^{N,L}", 100, 0., 0.);
	TProfile *p_y_F_L_vs_y_F_R = new TProfile("p_y_F_L_vs_y_F_R", ";y^{F,R};y^{F,L}", 100, 0., 0.);

	TProfile *p_x_N_L_vs_th_x_L = new TProfile("p_x_N_L_vs_th_x_L", ";#theta_{x}^{L};x^{N,L}", 100, 0., 0.);
	TProfile *p_x_F_L_vs_th_x_L = new TProfile("p_x_F_L_vs_th_x_L", ";#theta_{x}^{L};x^{F,L}", 100, 0., 0.);
	TProfile *p_x_N_R_vs_th_x_R = new TProfile("p_x_N_R_vs_th_x_R", ";#theta_{x}^{R};x^{N,R}", 100, 0., 0.);
	TProfile *p_x_F_R_vs_th_x_R = new TProfile("p_x_F_R_vs_th_x_R", ";#theta_{x}^{R};x^{F,R}", 100, 0., 0.);

	TProfile *p_y_N_L_vs_th_y_L = new TProfile("p_y_N_L_vs_th_y_L", ";#theta_{y}^{L};y^{N,L}", 100, 0., 0.);
	TProfile *p_y_F_L_vs_th_y_L = new TProfile("p_y_F_L_vs_th_y_L", ";#theta_{y}^{L};y^{F,L}", 100, 0., 0.);
	TProfile *p_y_N_R_vs_th_y_R = new TProfile("p_y_N_R_vs_th_y_R", ";#theta_{y}^{R};y^{N,R}", 100, 0., 0.);
	TProfile *p_y_F_R_vs_th_y_R = new TProfile("p_y_F_R_vs_th_y_R", ";#theta_{y}^{R};y^{F,R}", 100, 0., 0.);
	*/

	// zero counters
	unsigned long n_ev_full = 0;
	map<unsigned int, unsigned long> n_ev_cut;
	for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
		n_ev_cut[ci] = 0;

	double th_min = 1E100;
	double th_min_no_cut = 1E100;
	double th_y_L_min = +1E100, th_y_R_min = +1E100;

	unsigned int N_anal=0, N_anal_zeroBias=0;
	unsigned int N_zeroBias_el=0, N_zeroBias_el_RP_trig=0;
	unsigned int N_4outof4=0, N_el=0;
	unsigned int N_el_T2trig=0, N_4outof4_T2trig=0;
	unsigned int N_el_raw=0;

	map<unsigned int, pair<unsigned int, unsigned int> > runTimestampBoundaries;

	// build histograms - start event loop
	for (int ev_idx = 0; ev_idx < ch_in->GetEntries(); ev_idx += evIdxStep)
	{
		ch_in->GetEntry(ev_idx);

		// remove troublesome runs
		unsigned int run = ev.run_num / 100000;
		unsigned int file = ev.run_num % 100000;
		if (SkipRun(run, file, true))
			continue;

		// update timestamp run boundaries
		auto rtbit = runTimestampBoundaries.find(run);
		if (rtbit == runTimestampBoundaries.end())
		{
			runTimestampBoundaries.insert({run, {ev.timestamp, ev.timestamp}});
		} else {
			rtbit->second.first = min(rtbit->second.first, ev.timestamp);
			rtbit->second.second = max(rtbit->second.second, ev.timestamp);
		}

		// check time - selected?
		if (anal.SkipTime(ev.timestamp))
			continue;

		if (time_group_divisor != 0)
		{
			double time_group_interval = 1.;	// s
			int time_group = int(ev.timestamp / time_group_interval);
			if ( (time_group % time_group_divisor) != time_group_remainder)
				continue;
		}

		// fill trigger-bit histogram
		if (detailsLevel >= 2)
		{	
			for (unsigned int bit = 0; bit <= 15; bit++)
			{
				unsigned int val = (ev.trigger_bits >> bit) % 2;
				if (val)
					h_trigger_bit->Fill(bit);
			}
		}

		// apply fine alignment
		HitData h_al = ev.h;
		for (unsigned int i = 0; i < alignmentSources.size(); ++i)
		{
			AlignmentData alData = alignmentSources[i].Eval(ev.timestamp);
			h_al = h_al.ApplyAlignment(alData);
		}

		// fill pre-selection histograms
		h_y_L_1_F_vs_x_L_1_F_al_nosel->Fill(h_al.L_1_F.x, h_al.L_1_F.y);
		h_y_L_2_F_vs_x_L_2_F_al_nosel->Fill(h_al.L_2_F.x, h_al.L_2_F.y);
		h_y_R_1_F_vs_x_R_1_F_al_nosel->Fill(h_al.R_1_F.x, h_al.R_1_F.y);
		h_y_R_2_F_vs_x_R_2_F_al_nosel->Fill(h_al.R_2_F.x, h_al.R_2_F.y);

		// diagonal cut
		bool allDiagonalRPs = (ev.h.L_2_F.v && ev.h.L_1_F.v && ev.h.R_1_F.v && ev.h.R_2_F.v);
		if (!allDiagonalRPs)
			continue;

		if ((ev.trigger_bits & 64) != 0)
			N_4outof4_T2trig++;

		h_timestamp_dgn->Fill(ev.timestamp);
		//g_timestamp_vs_ev_idx_dgn->SetPoint(g_timestamp_vs_ev_idx_dgn->GetN(), ev_idx, ev.timestamp);

		// select the elastic-trigger bunch(es) only
		if (SkipBunch(run, ev.bunch_num))
			continue;

		// zero bias event?
		bool zero_bias_event = IsZeroBias(ev.trigger_bits, ev.run_num, ev.event_num);

		N_anal++;
		if (zero_bias_event)
			N_anal_zeroBias++;

		h_timestamp_B0->Fill(ev.timestamp);

		if (detailsLevel >= 2)
		{
			g_run_vs_timestamp->SetPoint(g_run_vs_timestamp->GetN(), ev.timestamp, ev.run_num);
			g_ev_num_vs_timestamp->SetPoint(g_ev_num_vs_timestamp->GetN(), ev.timestamp, ev.event_num);
			g_tr_num_vs_timestamp->SetPoint(g_tr_num_vs_timestamp->GetN(), ev.timestamp, ev.trigger_num);
		}

		// run reconstruction
		Kinematics k = DoReconstructionMain(h_al, env);
		Kinematics k_cuts = DoReconstructionForCuts(h_al, env);

		/*
		printf("------------------ event %i --------------\n", ev_idx);
		printf("th_x_L = %E, th_x_R = %E, th_x = %E\n", k.th_x_L, k.th_x_R, k.th_x);
		printf("th_y_L = %E, th_y_R = %E, th_y = %E\n", k.th_y_L, k.th_y_R, k.th_y);
		printf("L_1_F: x / L_x = %E\n", h_al.L_1_F.x / env.L_x_L_1_F);
		printf("L_2_F: x / L_x = %E\n", h_al.L_2_F.x / env.L_x_L_2_F);
		printf("R_1_F: x / L_x = %E\n", h_al.R_1_F.x / env.L_x_R_1_F);
		printf("R_2_F: x / L_x = %E\n", h_al.R_2_F.x / env.L_x_R_2_F);
		*/

		// alternative theta_x reconstruction
		/*
		// this is used in the old 90m analysis (for the PRL publication)
		double ta_x = (h_al.x_R_F - h_al.x_R_N - h_al.x_L_F + h_al.x_L_N) / (env.L_x_R_F - env.L_x_R_N) / 2.;
		*/

		// alternative theta_y reconstruction
		/*
		double D_y_L = - env.L_y_L_N * env.v_y_L_F + env.L_y_L_F * env.v_y_L_N;
		double ta_y_L = (env.v_y_L_F * h_al.y_L_N - env.v_y_L_N * h_al.y_L_F) / D_y_L;

		double D_y_R = env.L_y_R_N * env.v_y_R_F - env.L_y_R_F * env.v_y_R_N;
		double ta_y_R = (env.v_y_R_F * h_al.y_R_N - env.v_y_R_N * h_al.y_R_F) / D_y_R;

		double ta_y = (ta_y_L + ta_y_R) / 2.;
		*/

		// cut evaluation
		CutData cd;
		bool select = anal.EvaluateCuts(h_al, k_cuts, cd);
		//bool bckg = !select;

		// increment counters
		n_ev_full++;
		for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
		{
			if (cd.ct[ci])
				n_ev_cut[ci]++;
		}

		// fill no-cut histograms
		for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
			h2_cq_full[ci]->Fill(cd.cqa[ci], cd.cqb[ci]);

		N_4outof4++;

		// elastic cut
		if (!select)
			continue;

		N_el++;

		if (maxTaggedEvents > 0 && N_el > maxTaggedEvents)
			break;

		g_selected_bunch_num_vs_timestamp->SetPoint(g_selected_bunch_num_vs_timestamp->GetN(), ev.timestamp, ev.bunch_num);

		// fill zero-bias plots
		if (zero_bias_event)
		{
			N_zeroBias_el++;
			if ((ev.trigger_bits & 7) != 0)
				N_zeroBias_el_RP_trig++;
		}

		// elastic events with T2 trigger?
		if ((ev.trigger_bits & 64) != 0)
			N_el_T2trig++;

		// enforce the RP trigger (vertical OR horizontal)
		/*
		if ((ev.trigger_bits & 7) == 0)
			continue;
		*/

		N_el_raw++;

		// check event group
		if (event_group_divisor > 0)
		{
			int event_group = (N_el_raw-1) / event_group_divisor;
			if (event_group < event_group_index)
				continue;
			if (event_group > event_group_index)
				break;
		}

		// determine normalization factors (luminosity + corrections)
		double inefficiency_3outof4 = anal.inefficiency_3outof4;
		if (anal.use_3outof4_efficiency_fits)
		{
			const double th_y_abs = fabs(k.th_y);

			inefficiency_3outof4 = 0.;
			inefficiency_3outof4 += 1. - f_3outof4_efficiency_L_2_F->Eval(th_y_abs);
			inefficiency_3outof4 += 1. - f_3outof4_efficiency_L_1_F->Eval(th_y_abs);
			inefficiency_3outof4 += 1. - f_3outof4_efficiency_R_1_F->Eval(th_y_abs);
			inefficiency_3outof4 += 1. - f_3outof4_efficiency_R_2_F->Eval(th_y_abs);
		}

		double inefficiency_pile_up = anal.inefficiency_pile_up;
		if (anal.use_pileup_efficiency_fits)
			inefficiency_pile_up = corrg_pileup->Eval(ev.timestamp);

		double norm_corr =
			1./(1. - (inefficiency_3outof4 + anal.inefficiency_shower_near))
			* 1./(1. - inefficiency_pile_up)
			* 1./(1. - anal.inefficiency_trigger)
			* 1./(1. - anal.inefficiency_DAQ);

		p_norm_corr->Fill(ev.timestamp, norm_corr);
		p_3outof4_corr->Fill(k.th_y, inefficiency_3outof4);

		double normalization = anal.bckg_corr * norm_corr / anal.L_int;

		// data for alignment
		// (SHOULD use hit positions WITHOUT alignment corrections, i.e. ev.h)
		signed int period = int((ev.timestamp - anal.alignment_t0) / anal.alignment_ts);
		if (g_w_vs_timestamp_sel.find(period) == g_w_vs_timestamp_sel.end())
		{
			g_w_vs_timestamp_sel[period] = new TGraph();

			g_y_L_1_F_vs_x_L_1_F_sel[period] = new TGraph();
			g_y_L_2_F_vs_x_L_2_F_sel[period] = new TGraph();

			g_y_R_1_F_vs_x_R_1_F_sel[period] = new TGraph();
			g_y_R_2_F_vs_x_R_2_F_sel[period] = new TGraph();

			tm_h_th_x_L[period] = new TH1D("", ";#theta_{x}^{L}", 100, -150E-6, +150E-6);
			tm_h_th_x_R[period] = new TH1D("", ";#theta_{x}^{R}", 100, -150E-6, +150E-6);

			tm_p_diffLR_th_x[period] = new TProfile("", ";#theta_{x}   (#murad);#Delta^{R-L} #theta_{x}   (#murad)", 300, -300E-6, +300E-6);
			tm_p_diffLR_th_y[period] = new TProfile("", ";#theta_{y}   (#murad);#Delta^{R-L} #theta_{y}   (#murad)", 200, -500E-6, +500E-6);
		}

		if (detailsLevel >= 2)
		{
			int idx = g_w_vs_timestamp_sel[period]->GetN();
			g_w_vs_timestamp_sel[period]->SetPoint(idx, ev.timestamp, norm_corr);

			g_y_L_1_F_vs_x_L_1_F_sel[period]->SetPoint(idx, ev.h.L_1_F.x, ev.h.L_1_F.y);
			g_y_L_2_F_vs_x_L_2_F_sel[period]->SetPoint(idx, ev.h.L_2_F.x, ev.h.L_2_F.y);

			g_y_R_1_F_vs_x_R_1_F_sel[period]->SetPoint(idx, ev.h.R_1_F.x, ev.h.R_1_F.y);
			g_y_R_2_F_vs_x_R_2_F_sel[period]->SetPoint(idx, ev.h.R_2_F.x, ev.h.R_2_F.y);
		}

		// fill raw histograms
		h_timestamp_sel->Fill(ev.timestamp);
		if (detailsLevel >= 2)
		{
			g_timestamp_vs_ev_idx_sel->SetPoint(g_timestamp_vs_ev_idx_sel->GetN(), ev_idx, ev.timestamp);
			g_bunch_num_vs_timestamp->SetPoint(g_bunch_num_vs_timestamp->GetN(), ev.timestamp, ev.bunch_num);
		}

		for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
		{
			h_cq[ci]->Fill(cd.cv[ci]);
			h2_cq[ci]->Fill(cd.cqa[ci], cd.cqb[ci]);
			//h2_cq[ci]->Fill(ccb[ci]*cqa[ci] - cca[ci]*cqb[ci], cca[ci]*cqa[ci] + ccb[ci]*cqb[ci] + ccc[ci]);
			/*
			if (detailsLevel >= 2)
				g_cq[ci]->SetPoint(g_cq[ci]->GetN(), cd.cqa[ci], cd.cqb[ci]);
			*/
			p_cq[ci]->Fill(cd.cqa[ci], cd.cqb[ci]);
			p_cq_time[ci]->Fill(ev.timestamp, cd.cv[ci]);
		}

		h_y_L_1_F_vs_x_L_1_F_noal_sel->Fill(ev.h.L_1_F.x, ev.h.L_1_F.y);
		h_y_L_2_F_vs_x_L_2_F_noal_sel->Fill(ev.h.L_2_F.x, ev.h.L_2_F.y);
		h_y_R_1_F_vs_x_R_1_F_noal_sel->Fill(ev.h.R_1_F.x, ev.h.R_1_F.y);
		h_y_R_2_F_vs_x_R_2_F_noal_sel->Fill(ev.h.R_2_F.x, ev.h.R_2_F.y);

		h_y_L_1_F_vs_x_L_1_F_al_sel->Fill(h_al.L_1_F.x, h_al.L_1_F.y);
		h_y_L_2_F_vs_x_L_2_F_al_sel->Fill(h_al.L_2_F.x, h_al.L_2_F.y);
		h_y_R_1_F_vs_x_R_1_F_al_sel->Fill(h_al.R_1_F.x, h_al.R_1_F.y);
		h_y_R_2_F_vs_x_R_2_F_al_sel->Fill(h_al.R_2_F.x, h_al.R_2_F.y);

		/*
		{
			int idx = g_y_L_1_F_vs_x_L_1_F_al_sel->GetN();
			if (idx < 100000)
			{
				g_y_L_1_F_vs_x_L_1_F_al_sel->SetPoint(idx, h_al.L_1_F.x, h_al.L_1_F.y);
				g_y_L_2_F_vs_x_L_2_F_al_sel->SetPoint(idx, h_al.L_2_F.x, h_al.L_2_F.y);
				g_y_R_1_F_vs_x_R_1_F_al_sel->SetPoint(idx, h_al.R_1_F.x, h_al.R_1_F.y);
				g_y_R_2_F_vs_x_R_2_F_al_sel->SetPoint(idx, h_al.R_2_F.x, h_al.R_2_F.y);
			}
		}
		*/

		/*
		p_x_vs_y_L_F->Fill(h_al.y_L_F, h_al.x_L_F);
		p_x_vs_y_L_N->Fill(h_al.y_L_N, h_al.x_L_N);
		p_x_vs_y_R_N->Fill(h_al.y_R_N, h_al.x_R_N);
		p_x_vs_y_R_F->Fill(h_al.y_R_F, h_al.x_R_F);

		p_x_vs_y_L_F_noal->Fill(ev.h.y_L_F, ev.h.x_L_F);
		p_x_vs_y_L_N_noal->Fill(ev.h.y_L_N, ev.h.x_L_N);
		p_x_vs_y_R_N_noal->Fill(ev.h.y_R_N, ev.h.x_R_N);
		p_x_vs_y_R_F_noal->Fill(ev.h.y_R_F, ev.h.x_R_F);

		h_y_L_diffFN_vs_y_L_N->Fill(h_al.y_L_N, h_al.y_L_F - h_al.y_L_N);
		h_y_R_diffFN_vs_y_R_N->Fill(h_al.y_R_N, h_al.y_R_F - h_al.y_R_N);

		h_y_L_ratioFN_vs_y_L_N->Fill(h_al.y_L_N, h_al.y_L_F / h_al.y_L_N);
		h_y_R_ratioFN_vs_y_R_N->Fill(h_al.y_R_N, h_al.y_R_F / h_al.y_R_N);
		*/

		double safe_th_y_min = anal.fc_G_l.th_y_0 + 6E-6;
		double safe_th_y_max = anal.fc_G_h.th_y_0 - 5E-6;
		bool safe = fabs(k.th_y) > safe_th_y_min && fabs(k.th_y) < safe_th_y_max;

		if (safe)
		{
			th_x_diffLR->Fill(k.th_x_R - k.th_x_L);
			th_y_diffLR->Fill(k.th_y_R - k.th_y_L);

			th_x_diffLF->Fill(k.th_x_L - k.th_x);
			th_x_diffRF->Fill(k.th_x_R - k.th_x);

			h_th_x_diffLR_vs_th_x->Fill(k.th_x, k.th_x_R - k.th_x_L);
			h_th_y_diffLR_vs_th_y->Fill(k.th_y, k.th_y_R - k.th_y_L);
			h_th_x_diffLR_vs_vtx_x->Fill(k.vtx_x, k.th_x_R - k.th_x_L);

			p_th_x_diffLR_vs_th_x->Fill(k.th_x, k.th_x_R - k.th_x_L);
			p_th_x_L_diff12_vs_th_x_L->Fill(k.th_x_L, k.th_x_L_2_F - k.th_x_L_1_F);
			p_th_x_R_diff12_vs_th_x_R->Fill(k.th_x_R, k.th_x_R_2_F - k.th_x_R_1_F);

			p_th_y_diffLR_vs_th_y->Fill(k.th_y, k.th_y_R - k.th_y_L);
			p_th_y_L_diff12_vs_th_y_L->Fill(k.th_y_L, k.th_y_L_2_F - k.th_y_L_1_F);
			p_th_y_R_diff12_vs_th_y_R->Fill(k.th_y_R, k.th_y_R_2_F - k.th_y_R_1_F);

			p_th_x_diffLR_vs_vtx_x->Fill(k.vtx_x, k.th_x_R - k.th_x_L);
		}

		if (safe)
		{
			th_y_diffLR_safe->Fill(k.th_y_R - k.th_y_L);
			th_x_diffLR_safe->Fill(k.th_x_R - k.th_x_L);

			p_th_x_vs_th_y->Fill(k.th_y, k.th_x);
			p_th_x_L_vs_th_y_L->Fill(k.th_y_L, k.th_x_L);
			p_th_x_R_vs_th_y_R->Fill(k.th_y_R, k.th_x_R);
		}

		// these histograms are used to determine fiducial cuts - should be created WITHOUT the safe condition
		h_th_y_L_vs_th_x_L->Fill(k.th_x_L, k.th_y_L);
		h_th_y_R_vs_th_x_R->Fill(k.th_x_R, k.th_y_R);
		h_th_y_vs_th_x->Fill(k.th_x, k.th_y);

		if (detailsLevel >= 1)
		{
			g_th_y_L_vs_th_x_L->SetPoint(g_th_y_L_vs_th_x_L->GetN(), k.th_x_L, k.th_y_L);
			g_th_y_R_vs_th_x_R->SetPoint(g_th_y_R_vs_th_x_R->GetN(), k.th_x_R, k.th_y_R);
			g_th_y_vs_th_x->SetPoint(g_th_y_vs_th_x->GetN(), k.th_x, k.th_y);
		}

		h_th_y_L_vs_th_y_R->Fill(k.th_y_R, k.th_y_L);
		if (detailsLevel > 2)
			g_th_y_L_vs_th_y_R->SetPoint(g_th_y_L_vs_th_y_R->GetN(), k.th_y_R, k.th_y_L);

		h_th_x->Fill(k.th_x);
		h_th_y->Fill(k.th_y);
		h_th_y_flipped->Fill(-k.th_y);

		h_th_x_L->Fill(k.th_x_L);
		h_th_x_R->Fill(k.th_x_R);

		h_th_y_L->Fill(k.th_y_L);
		h_th_y_R->Fill(k.th_y_R);

		h_th_y_L_2_F->Fill(k.th_y_L_2_F);
		h_th_y_L_1_F->Fill(k.th_y_L_1_F);
		h_th_y_R_1_F->Fill(k.th_y_R_1_F);
		h_th_y_R_2_F->Fill(k.th_y_R_2_F);

		// fill vertex histograms

		h_vtx_x->Fill(k.vtx_x);
		h_vtx_x_L->Fill(k.vtx_x_L);
		h_vtx_x_R->Fill(k.vtx_x_R);

		h_vtx_y->Fill(k.vtx_y);
		h_vtx_y_L->Fill(k.vtx_y_L);
		h_vtx_y_R->Fill(k.vtx_y_R);

		h_vtx_x_L_vs_vtx_x_R->Fill(k.vtx_x_R, k.vtx_x_L);
		h_vtx_y_L_vs_vtx_y_R->Fill(k.vtx_y_R, k.vtx_y_L);

		h_vtx_x_L_vs_th_x_L->Fill(k.th_x_L, k.vtx_x_L);
		h_vtx_x_R_vs_th_x_R->Fill(k.th_x_R, k.vtx_x_R);
		h_vtx_y_L_vs_th_y_L->Fill(k.th_y_L, k.vtx_y_L);
		h_vtx_y_R_vs_th_y_R->Fill(k.th_y_R, k.vtx_y_R);

		h_vtx_x_diffLR->Fill(k.vtx_x_R - k.vtx_x_L);
		h_vtx_y_diffLR->Fill(k.vtx_y_R - k.vtx_y_L);

		h_vtx_x_diffLR_vs_th_x->Fill(k.th_x, k.vtx_x_R - k.vtx_x_L);
		h_vtx_y_diffLR_vs_th_y->Fill(k.th_y, k.vtx_y_R - k.vtx_y_L);

		p_vtx_x_diffLR_vs_th_x->Fill(k.th_x, k.vtx_x_R - k.vtx_x_L);
		p_vtx_y_diffLR_vs_th_y->Fill(k.th_y, k.vtx_y_R - k.vtx_y_L);

		h_vtx_x_diffLR_vs_vtx_x_R->Fill(k.vtx_x_R, k.vtx_x_R - k.vtx_x_L);
		h_vtx_y_diffLR_vs_vtx_y_R->Fill(k.vtx_y_R, k.vtx_y_R - k.vtx_y_L);

		if (safe)
		{
			h_vtx_x_safe->Fill(k.vtx_x);
			h_vtx_y_safe->Fill(k.vtx_y);

			h_vtx_x_diffLR_safe->Fill(k.vtx_x_R - k.vtx_x_L);
			h_vtx_y_diffLR_safe->Fill(k.vtx_y_R - k.vtx_y_L);

			h_vtx_x_diffLR_safe_corr->Fill((k.vtx_x_R - k.vtx_x_L) - 1080.*k.th_x);
			h_vtx_y_diffLR_safe_corr->Fill((k.vtx_y_R - k.vtx_y_L) + 156. *k.th_y);
		}

		if (true)
		{
			if (safe)
			{
				constexpr double d_RP = (220.000 - 213.000) * 1E3;	// mm

				const double thl_x_L = (h_al.L_2_F.x - h_al.L_1_F.x) / d_RP;
				const double thl_x_R = (h_al.R_2_F.x - h_al.R_1_F.x) / d_RP;

				const double thl_y_L = (h_al.L_2_F.y - h_al.L_1_F.y) / d_RP;
				const double thl_y_R = (h_al.R_2_F.y - h_al.R_1_F.y) / d_RP;

				opticsMatchingIntput_full.Fill(h_al, thl_x_L, thl_x_R, thl_y_L, thl_y_R, k_cuts.vtx_x_L, k_cuts.vtx_x_R);

				if (fabs(k_cuts.vtx_x) < 174E-3)
					opticsMatchingIntput_vtx_x_0p3sigma.Fill(h_al, thl_x_L, thl_x_R, thl_y_L, thl_y_R, k_cuts.vtx_x_L, k_cuts.vtx_x_R);

				if (fabs(k_cuts.vtx_x) < 58E-3)
					opticsMatchingIntput_vtx_x_0p1sigma.Fill(h_al, thl_x_L, thl_x_R, thl_y_L, thl_y_R, k_cuts.vtx_x_L, k_cuts.vtx_x_R);
			}
		}

		th_y_L_min = min(th_y_L_min, k.th_y_L);
		th_y_R_min = min(th_y_R_min, k.th_y_R);

		/*
		h_ta_th_x->Fill(ta_x - k.th_x);
		p_ta_th_x_vs_th_x->Fill(k.th_x, ta_x - k.th_x);

		h_ta_y_R->Fill(ta_y_R);
		h_ta_y_L->Fill(ta_y_L);

		h_ta_th_y_R->Fill(ta_y_R - k.th_y_R);
		h_ta_th_y_L->Fill(ta_y_L - k.th_y_L);
		h_ta_th_y->Fill(ta_y - k.th_y);
		h_ta_y_diffLR->Fill(ta_y_R - ta_y_L);

		h_ta_y_vs_th_y->Fill(k.th_y, ta_y);
		h_ta_y_L_vs_th_y_L->Fill(k.th_y_L, ta_y_L);
		h_ta_y_R_vs_th_y_R->Fill(k.th_y_R, ta_y_R);
		*/

		if (safe)
		{
			p_th_x_L1F_vs_time->Fill(ev.timestamp, k.th_x_L_1_F);
			p_th_x_L2F_vs_time->Fill(ev.timestamp, k.th_x_L_2_F);
			p_th_x_L_vs_time->Fill(ev.timestamp, k.th_x_L);
			p_th_x_R1F_vs_time->Fill(ev.timestamp, k.th_x_R_1_F);
			p_th_x_R2F_vs_time->Fill(ev.timestamp, k.th_x_R_2_F);
			p_th_x_R_vs_time->Fill(ev.timestamp, k.th_x_R);
			p_th_x_vs_time->Fill(ev.timestamp, k.th_x);

			p_th_y_L_vs_time->Fill(ev.timestamp, k.th_y_L);
			p_th_y_R_vs_time->Fill(ev.timestamp, k.th_y_R);
			p_th_y_vs_time->Fill(ev.timestamp, k.th_y);

			p_diffLR_th_x_vs_time->Fill(ev.timestamp, k.th_x_R - k.th_x_L);
			p_diffLR_th_y_vs_time->Fill(ev.timestamp, k.th_y_R - k.th_y_L);
		}

		if (safe)
		{
			p_kin_cuts_th_x_L_vs_time->Fill(ev.timestamp, k_cuts.th_x_L);
			p_kin_cuts_th_x_R_vs_time->Fill(ev.timestamp, k_cuts.th_x_R);
			p_kin_cuts_th_x_vs_time->Fill(ev.timestamp, k_cuts.th_x);

			p_kin_cuts_th_y_L_vs_time->Fill(ev.timestamp, k_cuts.th_y_L);
			p_kin_cuts_th_y_R_vs_time->Fill(ev.timestamp, k_cuts.th_y_R);
			p_kin_cuts_th_y_vs_time->Fill(ev.timestamp, k_cuts.th_y);

			p_kin_cuts_vtx_x_vs_time->Fill(ev.timestamp, k_cuts.vtx_x);

			p_kin_cuts_diffLR_th_x_vs_time->Fill(ev.timestamp, k_cuts.th_x_R - k_cuts.th_x_L);
			p_kin_cuts_diffLR_th_y_vs_time->Fill(ev.timestamp, k_cuts.th_y_R - k_cuts.th_y_L);

			p_kin_cuts_diff12_th_y_L_vs_time->Fill(ev.timestamp, k_cuts.th_y_L_2_F - k_cuts.th_y_L_1_F);
			p_kin_cuts_diff12_th_y_R_vs_time->Fill(ev.timestamp, k_cuts.th_y_R_2_F - k_cuts.th_y_R_1_F);
		}

		if (safe)
		{
			tm_h_th_x_L[period]->Fill(k.th_x_L);
			tm_h_th_x_R[period]->Fill(k.th_x_R);

			tm_p_diffLR_th_x[period]->Fill(k.th_x, k.th_x_R - k.th_x_L);
			tm_p_diffLR_th_y[period]->Fill(k.th_y, k.th_y_R - k.th_y_L);
		}

		// set time-dependent resolutions
		if (anal.use_resolution_fits)
		{
			anal.si_th_x_LRdiff = accCalc.anal.si_th_x_LRdiff = g_d_x_RMS->Eval(ev.timestamp);
			anal.si_th_y_LRdiff = accCalc.anal.si_th_y_LRdiff = g_d_y_RMS->Eval(ev.timestamp);
		}

		p_input_d_x_rms_vs_time->Fill(ev.timestamp, anal.si_th_x_LRdiff);
		p_input_d_y_rms_vs_time->Fill(ev.timestamp, anal.si_th_y_LRdiff);

		// calculate acceptance divergence correction
		double phi_corr = 0., div_corr = 0.;
		bool skip = accCalc.Calculate(k, phi_corr, div_corr);

		for (unsigned int bi = 0; bi < binnings.size(); bi++)
		{
			bh_t_Nev_before[bi]->Fill(k.t, 1.);
			bh_t_before[bi]->Fill(k.t, 1.);
		}

		h_th_y_vs_th_x_before->Fill(k.th_x, k.th_y, 1.);

		th_min_no_cut = min(th_min_no_cut, k.th);

		if (skip)
			continue;

		double corr = div_corr * phi_corr;

		th_min = min(th_min, k.th);

		// fill acceptance histograms
		p_t_ub_div_corr->Fill(k.t_y, div_corr);

		for (unsigned int bi = 0; bi < binnings.size(); bi++)
		{
			bh_t_Nev_after_no_corr[bi]->Fill(k.t, 1.);

			bh_t_after_no_corr[bi]->Fill(k.t, 1.);
			bh_t_after[bi]->Fill(k.t, corr);

			bp_t_phi_corr[bi]->Fill(k.t, phi_corr);
			bp_t_full_corr[bi]->Fill(k.t, corr);
		}

		h_th_y_vs_th_x_after->Fill(k.th_x, k.th_y, div_corr);
		h_th_vs_phi_after->Fill(k.phi, k.th, div_corr);

		if (detailsLevel >= 2)
			g_weight_vs_th_y->SetPoint(g_weight_vs_th_y->GetN(), k.th_y, div_corr);

		// apply normalization
		for (unsigned int bi = 0; bi < binnings.size(); bi++)
			bh_t_normalized[bi]->Fill(k.t, corr * normalization);

		h_th_y_vs_th_x_normalized->Fill(k.th_x, k.th_y, div_corr * normalization);

		if (detailsLevel >= 1)
		{
			g_th_y_vs_th_x_acc->SetPoint(g_th_y_vs_th_x_acc->GetN(), k.th_x, k.th_y);
			g_norm_corr_vs_div_corr->SetPoint(g_norm_corr_vs_div_corr->GetN(), div_corr, norm_corr);
		}
	}

	printf("---------------------------- after event loop ---------------------------\n");

	for (auto &p : runTimestampBoundaries)
	{
		printf("run %u: from %u to %u\n", p.first, p.second.first, p.second.second);
	}

	printf(">> th_min_no_cut = %E\n", th_min_no_cut);
	printf(">> th_min = %E\n", th_min);
	printf(">> th_y_L_min = %E\n", th_y_L_min);
	printf(">> th_y_R_min = %E\n", th_y_R_min);

	printf("\n");
	printf("N_anal = %u\n", N_anal);
	printf("N_anal_zeroBias = %u\n", N_anal_zeroBias);
	printf("N_zeroBias_el = %u\n", N_zeroBias_el);
	printf("N_zeroBias_el_RP_trig = %u\n", N_zeroBias_el_RP_trig);

	printf("N_4outof4 = %u\n", N_4outof4);
	printf("N_el = %u\n", N_el);
	printf("N_el_T2trig = %u\n", N_el_T2trig);
	printf("N_4outof4_T2trig = %u\n", N_4outof4_T2trig);

	// derived plots
	TGraphErrors *th_y_sigmaLR_vs_th_y = new TGraphErrors();
	th_y_sigmaLR_vs_th_y->SetName("th_y_sigmaLR_vs_th_y");
	th_y_sigmaLR_vs_th_y->SetTitle(";#theta_{y};RMS of #Delta^{R-L} #theta_{y}");
	ProfileToRMSGraph(p_th_y_diffLR_vs_th_y, th_y_sigmaLR_vs_th_y);

	TGraphErrors *th_x_sigmaLR_vs_th_x = new TGraphErrors();
	th_x_sigmaLR_vs_th_x->SetName("th_x_sigmaLR_vs_th_x");
	th_x_sigmaLR_vs_th_x->SetTitle(";#theta_{x};RMS of #Delta^{R-L} #theta_{x}");
	ProfileToRMSGraph(p_th_x_diffLR_vs_th_x, th_x_sigmaLR_vs_th_x);

	// normalize histograms
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		bh_t_before[bi]->Scale(1., "width");
		bh_t_after_no_corr[bi]->Scale(1., "width");
		bh_t_after[bi]->Scale(1., "width");

		bh_t_normalized[bi]->Scale(1., "width");
	}

	h_th_y_vs_th_x_normalized->Scale(1., "width");

	th_y_diffLR->Scale(1., "width");
	th_x_diffLR->Scale(1., "width");
	th_y_diffLR_safe->Scale(1., "width");
	th_x_diffLR_safe->Scale(1., "width");

	// hide bins with high uncertainty
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
		HideLowTBins(bh_t_normalized[bi], anal.t_min_fit);

	// fit histograms
	//double th_y_low_bound = (diagonal == d45b_56t) ? (anal.th_y_lcut_L+anal.th_y_lcut_R)/2. + 5E-6 : -((anal.th_y_hcut_L+anal.th_y_hcut_R)/2. - 5E-6);
	//double th_y_high_bound = (diagonal == d45b_56t) ? (anal.th_y_hcut_L+anal.th_y_hcut_R)/2. - 5E-6 : -((anal.th_y_lcut_L+anal.th_y_lcut_R)/2. + 5E-6);
	double th_y_low_bound = (diagonal == d45b_56t) ? 10E-6 : -90E-6;
	double th_y_high_bound = (diagonal == d45b_56t) ? 90E-6 : -10E-6;

	printf("\n* th_y fit bounds: from %E to %E\n", th_y_low_bound, th_y_high_bound);

	printf("\n* fitting p_th_x_diffLR_vs_th_x\n");
	p_th_x_diffLR_vs_th_x->Fit("pol1", "", "", -80E-6, +80E-6);
	printf("\n* fitting p_th_x_L_diff12_vs_th_x_L\n");
	p_th_x_L_diff12_vs_th_x_L->Fit("pol1", "", "", -80E-6, +80E-6);
	printf("\n* fitting p_th_x_R_diff12_vs_th_x_R\n");
	p_th_x_R_diff12_vs_th_x_R->Fit("pol1", "", "", -80E-6, +80E-6);

	printf("\n* fitting p_th_y_diffLR_vs_th_y\n");
	p_th_y_diffLR_vs_th_y->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);
	printf("\n* fitting p_th_y_L_diff12_vs_th_y_L\n");
	p_th_y_L_diff12_vs_th_y_L->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);
	printf("\n* fitting p_th_y_R_diff12_vs_th_y_R\n");
	p_th_y_R_diff12_vs_th_y_R->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);

	printf("* fitting p_vtx_x_diffLR_vs_th_x\n");
	p_vtx_x_diffLR_vs_th_x->Fit("pol1", "", "", -120E-6, +120E-6);

	printf("* fitting p_th_x_vs_th_y\n");
	p_th_x_vs_th_y->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);
	printf("* fitting p_th_x_L_vs_th_y_L\n");
	p_th_x_L_vs_th_y_L->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);
	printf("* fitting p_th_x_R_vs_th_y_R\n");
	p_th_x_R_vs_th_y_R->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);

	printf("* fitting h_th_x_R\n");
	h_th_x_R->Fit("gaus", "", "", -50E-6, +50E-6);
	printf("* fitting h_th_x_L\n");
	h_th_x_L->Fit("gaus", "", "", -50E-6, +50E-6);

	printf("* fitting p_th_x_R_vs_time\n");
	p_th_x_R_vs_time->Fit("pol1");
	printf("* fitting p_th_y_R_vs_time\n");
	p_th_y_R_vs_time->Fit("pol1");
	printf("* fitting p_th_x_L_vs_time\n");
	p_th_x_L_vs_time->Fit("pol1");
	printf("* fitting p_th_y_L_vs_time\n");
	p_th_y_L_vs_time->Fit("pol1");

	// double-gauss fit for background histograms
	/*
	//TF1 *dg = new TF1("dg", "[0]*exp(-(x-[1])*(x-[1])/2/[2]/[2]) + [3]*exp(-(x-[4])*(x-[4])/2/[5]/[5])");
	for (map<unsigned int, TH1D *>::iterator it = hb_cq.begin(); it != hb_cq.end(); ++it)
	{
		printf("* Fitting background projection %u\n", it->first);
		dg->SetParameters(5E3, 0., 1., 1E2, 0., 3.);
		it->second->Fit(dg, "", "", -9., +9.);

		double A = dg->GetParameter(0), mu = dg->GetParameter(1), si = dg->GetParameter(2);
		double signal_int = A*si*sqrt(2.*M_PI)/2. * ( TMath::Erf((+3. - mu)/sqrt(2.)/si) - TMath::Erf((-3. - mu)/sqrt(2.)/si) );

		A = dg->GetParameter(3); mu = dg->GetParameter(4); si = dg->GetParameter(5);
		double background_int = A*si*sqrt(2.*M_PI)/2. * ( TMath::Erf((+3. - mu)/sqrt(2.)/si) - TMath::Erf((-3. - mu)/sqrt(2.)/si) );
		printf("\tsignal int = %E\n", signal_int);
		printf("\tbackground int = %E\n", background_int);
		printf("\tsignal + background int = %E\n", signal_int + background_int);
		printf("\tbackground/signal = %E\n", background_int / signal_int);
	}
	*/

	/*
	printf("* fitting p_x_vs_y_L_F\n");
	p_x_vs_y_L_F->Fit("pol1", "", "", -15., +15.);
	printf("* fitting p_x_vs_y_L_N\n");
	p_x_vs_y_L_N->Fit("pol1", "", "", -15., +15.);
	printf("* fitting p_x_vs_y_R_N\n");
	p_x_vs_y_R_N->Fit("pol1", "", "", -15., +15.);
	printf("* fitting p_x_vs_y_R_F\n");
	p_x_vs_y_R_F->Fit("pol1", "", "", -15., +15.);
	*/

	th_y_diffLR_safe->Fit("gaus");
	th_x_diffLR_safe->Fit("gaus");

	// apply unfolding correction
	map<unsigned int, TH1D *>  bh_t_normalized_unsmeared;
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		bh_t_normalized_unsmeared[bi] = new TH1D(* bh_t_normalized[bi]);
		bh_t_normalized_unsmeared[bi]->SetName("h_t_normalized_unsmeared");

		for (int bin = 1; bin <= bh_t_normalized_unsmeared[bi]->GetNbinsX(); ++bin)
		{
			double v = bh_t_normalized_unsmeared[bi]->GetBinContent(bin);
			double v_u = bh_t_normalized_unsmeared[bi]->GetBinError(bin);

			double corr = (apply_unsmearing) ? unsmearing_corrections[bi]->GetBinContent(bin) : 0.;

			bh_t_normalized_unsmeared[bi]->SetBinContent(bin, v * corr);
			bh_t_normalized_unsmeared[bi]->SetBinError(bin, v_u * corr);
		}
	}

	// derived plots
	map<unsigned int, TGraphErrors *> bh_t_normalized_rel_diff, bh_t_normalized_unsmeared_rel_diff;
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		bh_t_normalized_rel_diff[bi] = MakeRelDiff(bh_t_normalized[bi]);
		bh_t_normalized_rel_diff[bi]->SetName("h_t_eb_normalized_rel_diff");

		bh_t_normalized_unsmeared_rel_diff[bi] = MakeRelDiff(bh_t_normalized_unsmeared[bi]);
		bh_t_normalized_unsmeared_rel_diff[bi]->SetName("h_t_eb_normalized_unsmeared_rel_diff");
	}

	// save histograms
	TCanvas *c;

	gDirectory = outF->mkdir("metadata");

	c = new TCanvas("rate cmp");
	h_timestamp_dgn->Draw();
	h_timestamp_B0->SetLineColor(4);
	h_timestamp_B0->Draw("sames");
	h_timestamp_sel->SetLineColor(2);
	h_timestamp_sel->Draw("sames");
	c->Write();

	if (detailsLevel >= 2)
	{	
		h_trigger_bit->Write();

		//g_timestamp_vs_ev_idx_dgn->Write();
		g_timestamp_vs_ev_idx_sel->Write();

		g_run_vs_timestamp->Write();
		g_ev_num_vs_timestamp->Write();
		g_tr_num_vs_timestamp->Write();
		g_bunch_num_vs_timestamp->Write();
		g_selected_bunch_num_vs_timestamp->Write();
	}

	TDirectory *hitDistDir = outF->mkdir("hit distributions");
	gDirectory = hitDistDir->mkdir("vertical, aligned, before selection");
	h_y_L_2_F_vs_x_L_2_F_al_nosel->Write();
	h_y_L_1_F_vs_x_L_1_F_al_nosel->Write();
	h_y_R_1_F_vs_x_R_1_F_al_nosel->Write();
	h_y_R_2_F_vs_x_R_2_F_al_nosel->Write();

	gDirectory = hitDistDir->mkdir("vertical, not aligned, after selection");
	h_y_L_2_F_vs_x_L_2_F_noal_sel->Write();
	h_y_L_1_F_vs_x_L_1_F_noal_sel->Write();
	h_y_R_1_F_vs_x_R_1_F_noal_sel->Write();
	h_y_R_2_F_vs_x_R_2_F_noal_sel->Write();

	gDirectory = hitDistDir->mkdir("vertical, aligned, after selection");
	h_y_L_2_F_vs_x_L_2_F_al_sel->Write();
	h_y_L_1_F_vs_x_L_1_F_al_sel->Write();
	h_y_R_1_F_vs_x_R_1_F_al_sel->Write();
	h_y_R_2_F_vs_x_R_2_F_al_sel->Write();

	/*
	g_y_L_2_F_vs_x_L_2_F_al_sel->Write();
	g_y_L_1_F_vs_x_L_1_F_al_sel->Write();
	g_y_R_1_F_vs_x_R_1_F_al_sel->Write();
	g_y_R_2_F_vs_x_R_2_F_al_sel->Write();
	*/

	TDirectory *alDir = outF->mkdir("alignment");

	TF1 *ff = new TF1("ff", "[0] + [1]*x");

	TGraphErrors* g_ext_diffLR_th_x_vs_time = new TGraphErrors(); g_ext_diffLR_th_x_vs_time->SetName("g_ext_diffLR_th_x_vs_time");
	TGraphErrors* g_ext_diffLR_th_y_vs_time = new TGraphErrors(); g_ext_diffLR_th_y_vs_time->SetName("g_ext_diffLR_th_y_vs_time");

	for (map<signed int, TGraph *>::iterator pid = g_w_vs_timestamp_sel.begin(); pid != g_w_vs_timestamp_sel.end(); ++pid)
	{
		signed int period = pid->first;

		char buf[100];
		sprintf(buf, "%i", period);
		gDirectory = alDir->mkdir(buf);

		g_w_vs_timestamp_sel[period]->SetName("g_w_vs_timestamp_sel"); g_w_vs_timestamp_sel[period]->Write();

		g_y_L_1_F_vs_x_L_1_F_sel[period]->SetName("g_y_L_1_F_vs_x_L_1_F_sel"); g_y_L_1_F_vs_x_L_1_F_sel[period]->Write();
		g_y_L_2_F_vs_x_L_2_F_sel[period]->SetName("g_y_L_2_F_vs_x_L_2_F_sel"); g_y_L_2_F_vs_x_L_2_F_sel[period]->Write();

		g_y_R_1_F_vs_x_R_1_F_sel[period]->SetName("g_y_R_1_F_vs_x_R_1_F_sel"); g_y_R_1_F_vs_x_R_1_F_sel[period]->Write();
		g_y_R_2_F_vs_x_R_2_F_sel[period]->SetName("g_y_R_2_F_vs_x_R_2_F_sel"); g_y_R_2_F_vs_x_R_2_F_sel[period]->Write();

		tm_h_th_x_L[period]->Write("tm_h_th_x_L");
		tm_h_th_x_R[period]->Write("tm_h_th_x_R");

		TProfile *p_th_x = tm_p_diffLR_th_x[period];
		p_th_x->SetName("p_diffLR_th_x");
		unsigned int reasonableBins_x = SuppressLowStatisticsBins(p_th_x, 5);
		p_th_x->Fit(ff, "Q");
		p_th_x->Write();
		double v_x = ff->GetParameter(0), u_x = ff->GetParError(0);

		TProfile *p_th_y = tm_p_diffLR_th_y[period];
		p_th_y->SetName("p_diffLR_th_y");
		unsigned int reasonableBins_y = SuppressLowStatisticsBins(p_th_y, 5);
		p_th_y->Fit(ff, "Q");
		p_th_y->Write();
		double v_y = ff->GetParameter(0), u_y = ff->GetParError(0);

		double time = anal.alignment_t0 + (period + 0.5) * anal.alignment_ts;
		double time_beg = anal.alignment_t0 + (period + 0.0) * anal.alignment_ts;
		double time_end = anal.alignment_t0 + (period + 1.0) * anal.alignment_ts;
		printf("period %u: from %.0f to %0.f\n", period, time_beg, time_end);

		if (reasonableBins_x > 9)
		{
			int idx = g_ext_diffLR_th_x_vs_time->GetN();
			g_ext_diffLR_th_x_vs_time->SetPoint(idx, time, v_x);
			g_ext_diffLR_th_x_vs_time->SetPointError(idx, 0., u_x);
		}

		if (reasonableBins_y > 9)
		{
			int idx = g_ext_diffLR_th_y_vs_time->GetN();
			g_ext_diffLR_th_y_vs_time->SetPoint(idx, time, v_y);
			g_ext_diffLR_th_y_vs_time->SetPointError(idx, 0., u_y);
		}
	}

	TDirectory *cutDir = outF->mkdir("elastic cuts");
	for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
	{
		char buf[100];
		sprintf(buf, "cut %u", ci);
		gDirectory = cutDir->mkdir(buf);

		printf("* RMS of cut distribution %u\n", ci);
		printf("%E\n", h_cq[ci]->GetRMS());

		printf("* fitting cut distribution %u\n", ci);
		h_cq[ci]->Fit("gaus", "", "", -3., +3.);

		h_cq[ci]->Write();
		h2_cq[ci]->Write();
		h2_cq_full[ci]->Write();
		//g_cq[ci]->Write();

		p_cq[ci]->Write();

		p_cq_time[ci]->Write();

		TGraphErrors *g_cq_time = new TGraphErrors();
		sprintf(buf, "g_cq_RMS%u", ci); g_cq_time->SetName(buf);
		sprintf(buf, ";time   (s);RMS of cq%u", ci); g_cq_time->SetTitle(buf);
		ProfileToRMSGraph(p_cq_time[ci], g_cq_time);
		g_cq_time->Write();

		h2_cq_full[ci]->Draw();

		sprintf(buf, "plot_before_cq%u", ci);
		c = new TCanvas(buf);
		c->SetLogz(1);
		h2_cq_full[ci]->Draw("colz");

		double lim = h2_cq_full[ci]->GetXaxis()->GetXmax();
		double qa[2] = {-lim, +lim};
		double qbp[2]= {(+anal.n_si*anal.csi[ci] - anal.cca[ci]*qa[0] - anal.ccc[ci])/anal.ccb[ci],
			(+anal.n_si*anal.csi[ci] - anal.cca[ci]*qa[1] - anal.ccc[ci])/anal.ccb[ci]};
		double qbm[2]= {(-anal.n_si*anal.csi[ci] - anal.cca[ci]*qa[0] - anal.ccc[ci])/anal.ccb[ci],
			(-anal.n_si*anal.csi[ci] - anal.cca[ci]*qa[1] - anal.ccc[ci])/anal.ccb[ci]};
		TGraph *gP = new TGraph(2, qa, qbp); gP->Draw("l");
		TGraph *gM = new TGraph(2, qa, qbm); gM->Draw("l");
		c->Write();

		sprintf(buf, "plot_after_cq%u", ci);
		c = new TCanvas(buf);
		c->SetLogz(1);
		h2_cq[ci]->Draw("colz");
		gP = new TGraph(2, qa, qbp); gP->Draw("l");
		gM = new TGraph(2, qa, qbm); gM->Draw("l");
		gP->Draw("l");
		gM->Draw("l");
		c->Write();

		TGraph *g_cut_parameters = new TGraph();
		g_cut_parameters->SetPoint(0, 0, anal.cca[ci]);
		g_cut_parameters->SetPoint(1, 1, anal.ccb[ci]);
		g_cut_parameters->SetPoint(2, 2, anal.ccc[ci]);
		g_cut_parameters->SetPoint(3, 3, anal.csi[ci]);
		g_cut_parameters->SetPoint(4, 4, anal.n_si);
		g_cut_parameters->Write("g_cut_parameters");
	}

	gDirectory = outF->mkdir("selected - hits");
	/*
	p_x_vs_y_L_F->Write();
	p_x_vs_y_L_N->Write();
	p_x_vs_y_R_N->Write();
	p_x_vs_y_R_F->Write();

	p_x_vs_y_L_F_noal->Write();
	p_x_vs_y_L_N_noal->Write();
	p_x_vs_y_R_N_noal->Write();
	p_x_vs_y_R_F_noal->Write();

	h_y_L_diffFN_vs_y_L_N->Write();
	h_y_R_diffFN_vs_y_R_N->Write();

	h_y_L_ratioFN_vs_y_L_N->Write();
	h_y_R_ratioFN_vs_y_R_N->Write();
	*/

	gDirectory = outF->mkdir("selected - angles");
	th_x_diffLR->Write();
	th_y_diffLR->Write();

	th_x_diffLF->Write();
	th_x_diffRF->Write();

	h_th_x_diffLR_vs_th_x->Write();
	h_th_y_diffLR_vs_th_y->Write();
	h_th_x_diffLR_vs_vtx_x->Write();

	p_th_x_diffLR_vs_th_x->Write();
	p_th_x_L_diff12_vs_th_x_L->Write();
	p_th_x_R_diff12_vs_th_x_R->Write();

	p_th_y_diffLR_vs_th_y->Write();
	p_th_y_L_diff12_vs_th_y_L->Write();
	p_th_y_R_diff12_vs_th_y_R->Write();

	p_th_x_diffLR_vs_vtx_x->Write();

	th_x_sigmaLR_vs_th_x->Write();
	th_y_sigmaLR_vs_th_y->Write();

	th_x_diffLR_safe->Write();
	th_y_diffLR_safe->Write();

	p_th_x_vs_th_y->Write();
	p_th_x_L_vs_th_y_L->Write();
	p_th_x_R_vs_th_y_R->Write();

	h_th_y_L_vs_th_x_L->Write();
	h_th_y_R_vs_th_x_R->Write();
	h_th_y_vs_th_x->Write();

	g_th_y_L_vs_th_x_L->Write();
	g_th_y_R_vs_th_x_R->Write();
	g_th_y_vs_th_x->Write();

	{
		c = new TCanvas();
		c->SetLogz(1);
		c->ToggleEventStatus();
		c->SetCrosshair(1);
		h_th_y_L_vs_th_y_R->Draw("colz");
		g_th_y_L_vs_th_y_R->Draw("p");
		c->Write("canvas_th_y_L_vs_th_y_R");
	}

	h_th_x->Write();
	h_th_y->Write();
	h_th_y_flipped->Write();

	h_th_x_L->Write();
	h_th_x_R->Write();

	h_th_y_L->Write();
	h_th_y_R->Write();

	h_th_y_L_2_F->Write();
	h_th_y_L_1_F->Write();
	h_th_y_R_1_F->Write();
	h_th_y_R_2_F->Write();

	/*
	gDirectory = outF->mkdir("selected - angles, alternative");

	h_ta_th_x_R->Write();
	h_ta_th_x_L->Write();
	h_ta_th_x->Write();
	h_ta_x_diffLR->Write();

	h_ta_y_R->Write();
	h_ta_y_L->Write();

	h_ta_th_y_R->Write();
	h_ta_th_y_L->Write();
	h_ta_th_y->Write();
	h_ta_y_diffLR->Write();

	h_ta_y_vs_th_y->Write();
	h_ta_y_L_vs_th_y_L->Write();
	h_ta_y_R_vs_th_y_R->Write();
	*/

	gDirectory = outF->mkdir("selected - vertex");
	h_vtx_x->Write();
	h_vtx_x_L->Write();
	h_vtx_x_R->Write();

	h_vtx_y->Write();
	h_vtx_y_L->Write();
	h_vtx_y_R->Write();

	h_vtx_x_safe->Write();
	h_vtx_y_safe->Write();

	h_vtx_x_L_vs_vtx_x_R->Write();
	h_vtx_y_L_vs_vtx_y_R->Write();

	h_vtx_x_L_vs_th_x_L->Write();
	h_vtx_x_R_vs_th_x_R->Write();
	h_vtx_y_L_vs_th_y_L->Write();
	h_vtx_y_R_vs_th_y_R->Write();

	h_vtx_x_diffLR->Write();
	h_vtx_y_diffLR->Write();

	h_vtx_x_diffLR_safe->Write();
	h_vtx_y_diffLR_safe->Write();

	h_vtx_x_diffLR_safe_corr->Write();
	h_vtx_y_diffLR_safe_corr->Write();

	h_vtx_x_diffLR_vs_th_x->Write();
	h_vtx_y_diffLR_vs_th_y->Write();

	p_vtx_x_diffLR_vs_th_x->Write();
	p_vtx_y_diffLR_vs_th_y->Write();

	h_vtx_x_diffLR_vs_vtx_x_R->Write();
	h_vtx_y_diffLR_vs_vtx_y_R->Write();

	/*
	p_x_L_F_vs_th_x->Write();
	p_x_L_N_vs_th_x->Write();
	p_x_R_F_vs_th_x->Write();
	p_x_R_N_vs_th_x->Write();

	p_x_L_F_vs_vtx_x->Write();
	p_x_L_N_vs_vtx_x->Write();
	p_x_R_F_vs_vtx_x->Write();
	p_x_R_N_vs_vtx_x->Write();

	p_vtx_x_L_vs_th_x_L->Write();
	p_vtx_x_L_vs_th_x->Write();
	p_vtx_x_R_vs_th_x_R->Write();
	p_vtx_x_R_vs_th_x->Write();
	*/


	TDirectory *opticsDir = outF->mkdir("optics");

	gDirectory = opticsDir->mkdir("matching input, full");
	opticsMatchingIntput_full.Write();

	gDirectory = opticsDir->mkdir("matching input, vtx_x_0.3sigma");
	opticsMatchingIntput_vtx_x_0p3sigma.Write();

	gDirectory = opticsDir->mkdir("matching input, vtx_x_0.1sigma");
	opticsMatchingIntput_vtx_x_0p1sigma.Write();

	gDirectory = outF->mkdir("binning");
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		TGraph *g = new TGraph();
		g->SetName(("g_binning_"+binnings[bi]).c_str());
		g->SetTitle(";bin center;bin width");

		TH1D *h = bh_t_Nev_before[bi];
		for (int bin = 1; bin <= h->GetNbinsX(); bin++)
			g->SetPoint(g->GetN(), h->GetBinCenter(bin), h->GetBinWidth(bin));

		g->Write();
	}

	gDirectory = outF->mkdir("time dependences");

	TGraph *g_run_boundaries = new TGraph();
	g_run_boundaries->SetName("g_run_boundaries");
	g_run_boundaries->SetTitle(";timestamp;run");
	for (auto &p : runTimestampBoundaries)
	{
		const int idx = g_run_boundaries->GetN();
		g_run_boundaries->SetPoint(idx, p.second.first, p.first);
		g_run_boundaries->SetPoint(idx+1, p.second.second, p.first);
	}
	g_run_boundaries->Write();

	for (TProfile *p : 
		{
			p_th_x_L1F_vs_time, p_th_x_L2F_vs_time, p_th_x_L_vs_time, p_th_x_R1F_vs_time, p_th_x_R2F_vs_time, p_th_x_R_vs_time, p_th_x_vs_time,
			p_th_y_L_vs_time, p_th_y_R_vs_time, p_th_y_vs_time,
			p_diffLR_th_x_vs_time, p_diffLR_th_y_vs_time,

			p_kin_cuts_th_x_L_vs_time, p_kin_cuts_th_x_R_vs_time, p_kin_cuts_th_x_vs_time,
			p_kin_cuts_th_y_L_vs_time, p_kin_cuts_th_y_R_vs_time, p_kin_cuts_th_y_vs_time,
			p_kin_cuts_vtx_x_vs_time,
			p_kin_cuts_diffLR_th_x_vs_time, p_kin_cuts_diffLR_th_y_vs_time, p_kin_cuts_diff12_th_y_L_vs_time, p_kin_cuts_diff12_th_y_R_vs_time
		} )
	{
		p->Write();

		string name = p->GetName();
		name = name.replace(0, 1, "gRMS");

		TGraphErrors *g = new TGraphErrors;
		g->SetName(name.c_str());
		//gRMS_diffLR_th_x_vs_time->SetTitle(";timestamp;RMS of #Delta^{R-L}#theta_{x}");
		ProfileToRMSGraph(p, g);
		g->Write();
	}

	p_input_d_x_rms_vs_time->Write();
	p_input_d_y_rms_vs_time->Write();

	g_ext_diffLR_th_x_vs_time->Write();
	g_ext_diffLR_th_y_vs_time->Write();

	TDirectory *fidCutDir = outF->mkdir("fiducial cuts");
	gDirectory = fidCutDir;
	PlotFiductialCut(anal.fc_L_l, th_y_sign)->Write("fc_L_l");
	PlotFiductialCut(anal.fc_L_h, th_y_sign)->Write("fc_L_h");
	PlotFiductialCut(anal.fc_R_l, th_y_sign)->Write("fc_R_l");
	PlotFiductialCut(anal.fc_R_h, th_y_sign)->Write("fc_R_h");
	PlotFiductialCut(anal.fc_G_l, th_y_sign)->Write("fc_G_l");
	PlotFiductialCut(anal.fc_G_h, th_y_sign)->Write("fc_G_h");

	TDirectory *accDir = outF->mkdir("acceptance correction");
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		gDirectory = accDir->mkdir(binnings[bi].c_str());
		bh_t_Nev_before[bi]->Write();
		bh_t_Nev_after_no_corr[bi]->Write();
		bh_t_before[bi]->Write();
		bh_t_after_no_corr[bi]->Write();
		bh_t_after[bi]->Write();
		bp_t_phi_corr[bi]->Write();
		bp_t_full_corr[bi]->Write();

		c = new TCanvas("t cmp");
		c->SetLogy(1);
		bh_t_after[bi]->Draw("");
		bh_t_before[bi]->Draw("same");
		c->Write();
	}

	gDirectory = accDir;

	p_t_ub_div_corr->Write();

	h_th_y_vs_th_x_before->Write();
	h_th_y_vs_th_x_after->Write();
	h_th_vs_phi_after->Write();

	g_weight_vs_th_y->Write();

	g_th_y_vs_th_x_acc->Write();

	TDirectory *normDir = outF->mkdir("normalization");
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		gDirectory = normDir->mkdir(binnings[bi].c_str());
		bh_t_normalized[bi]->Write();

		TH1D *h_t_normalized_no_L = new TH1D(* bh_t_normalized[bi]);
		h_t_normalized_no_L->Scale(anal.L_int);
		h_t_normalized_no_L->Write("h_t_normalized_no_L");

		bh_t_normalized_rel_diff[bi]->Write();
	}

	gDirectory = normDir;
	p_norm_corr->Write();
	p_3outof4_corr->Write();

	h_th_y_vs_th_x_normalized->Write();

	g_norm_corr_vs_div_corr->Write();

	TDirectory *normUnfDir = outF->mkdir("normalization+unfolding");
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		gDirectory = normUnfDir->mkdir(binnings[bi].c_str());

		bh_t_normalized_unsmeared[bi]->Write();
		bh_t_normalized_unsmeared_rel_diff[bi]->Write();
	}

	/*
	gDirectory = outF->mkdir("matching input");
	g_th_x_L_vs_th_x_R->Write();
	g_th_x_R_vs_th_x_L->Write();

	g_th_y_L_vs_th_y_R->Write();
	g_th_y_R_vs_th_y_L->Write();

	p_th_x_L_vs_th_x_R->Write();
	p_th_y_L_vs_th_y_R->Write();

	p_x_N_L_vs_x_N_R->Write();
	p_x_F_L_vs_x_F_R->Write();
	p_y_N_L_vs_y_N_R->Write();
	p_y_F_L_vs_y_F_R->Write();

	p_x_N_L_vs_th_x_L->Write();
	p_x_F_L_vs_th_x_L->Write();
	p_x_N_R_vs_th_x_R->Write();
	p_x_F_R_vs_th_x_R->Write();

	p_y_N_L_vs_th_y_L->Write();
	p_y_F_L_vs_th_y_L->Write();
	p_y_N_R_vs_th_y_R->Write();
	p_y_F_R_vs_th_y_R->Write();

	p_x_vs_y_L_F->Write();
	p_x_vs_y_L_N->Write();
	p_x_vs_y_R_N->Write();
	p_x_vs_y_R_F->Write();
	*/

	// print counters
	for (map<unsigned int, unsigned long>::iterator it = n_ev_cut.begin(); it != n_ev_cut.end(); ++it)
		printf("\tcut %u: %lu\n", it->first, it->second);

	return 0;
}
