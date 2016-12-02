#include "TRandom2.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TRandom2.h"
#include "TVectorD.h"
#include "TFile.h"

#include <string>
#include <vector>
#include <cmath>

#include "../stat.h"
#include "../common_definitions.h"
#include "../common_algorithms.h"

#include "formulae.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

typedef Kinematics (*Func)(const HitData &, const Environment &);

enum RecoQuantity
{
	rqTh_x = 0, rqTh_x_L, rqTh_x_R, rqTh_x_RLdiff, rqTh_x_LFdiff, rqTh_x_RFdiff, rqTh_x_LtoRratio,
	rqVtx_x = 100, rqVtx_x_L, rqVtx_x_R, rqVtx_x_RLdiff,
	rqTh_y = 200, rqTh_y_L, rqTh_y_R, rqTh_y_RLdiff, rqTh_y_LtoRratio
};

enum SimuBits { sbPitch=1, sbBeamDivergence=2, sbMisalignment=4, sbVertex=8, sbAngle=16, sbOptics=32 };

//----------------------------------------------------------------------------------------------------

Environment env_nom;

//----------------------------------------------------------------------------------------------------

void AddString(string &s, const string &add)
{
	if (s.empty())
		s = add;
	else
		s += "," + add;
}

//----------------------------------------------------------------------------------------------------

string SimuCodeToString(unsigned int code)
{
	string s;

	if (code & sbPitch) AddString(s, "pitch");
	if (code & sbBeamDivergence) AddString(s, "beamDiv");
	if (code & sbMisalignment) AddString(s, "misalig");
	if (code & sbVertex) AddString(s, "vertex");
	if (code & sbAngle) AddString(s, "angle");
	if (code & sbOptics) AddString(s, "optics");

	return s;
}

//----------------------------------------------------------------------------------------------------

Stat st_opt(16);

void GenerateOpticsPerturbation(Environment &env)
{
	TVectorD de(16);
	env.ApplyRandomOpticsPerturbations(de);
	st_opt.Fill(de);
}

//---------------------------------------------------------------------------------------------------

void TestOnePoint(RecoQuantity q, Func f_reco, unsigned int b, const Kinematics k_cv,
		double &mean, double &stddev)
{
	// effect histogram
	TH1D *h_eff = new TH1D("", "", 100, 0., 0.);

	// event loop
	unsigned int N_ev = 10000;
	for (unsigned int ev = 0; ev < N_ev; ev++)
	{
		// true
		Kinematics k_tr = k_cv;

		// generate scattering angles
		if (b & sbAngle)
		{
			double B = 20.;
			double si_th_x = sqrt(1. / (2. * B * env_nom.p * env_nom.p)), si_th_y = si_th_x;
			k_tr.th_x = k_tr.th_x_L = k_tr.th_x_R = gRandom->Gaus() * si_th_x;
			k_tr.th_y = k_tr.th_y_L = k_tr.th_y_R = gRandom->Gaus() * si_th_y;
		}

		// simulation environment
		Environment env_sim = env_nom;

		// beam divergence + vertex
		Kinematics k_sm = k_tr;
		if (b & sbBeamDivergence)
		{
			k_sm.th_x_L += gRandom->Gaus() * env_sim.si_th_x_L;
			k_sm.th_y_L += gRandom->Gaus() * env_sim.si_th_y_L;
			k_sm.th_x_R += gRandom->Gaus() * env_sim.si_th_x_R;
			k_sm.th_y_R += gRandom->Gaus() * env_sim.si_th_y_R;
		}

		if (b & sbVertex)
		{
			k_sm.vtx_x += gRandom->Gaus() * env_sim.si_vtx_x;
			k_sm.vtx_y += gRandom->Gaus() * env_sim.si_vtx_y;
		}

		// actual optics
		if (b & sbOptics)
		{
			GenerateOpticsPerturbation(env_sim);
		}

		// proton transport
		HitData h = ProtonTransport(k_sm, env_sim);
		
		// misalignment
		if (b & sbMisalignment) 
		{
			double de_y_R = gRandom->Gaus() * env_sim.si_de_y_R;
			double de_y_D = gRandom->Gaus() * env_sim.si_de_y_D;

			h.L_2_F.x += gRandom->Gaus() * env_sim.si_de_x; h.L_2_F.y += -de_y_R + de_y_D;
			h.L_1_F.x += gRandom->Gaus() * env_sim.si_de_x; h.L_1_F.y += -de_y_R + de_y_D;
			h.R_1_F.x += gRandom->Gaus() * env_sim.si_de_x; h.R_1_F.y += +de_y_R;
			h.R_2_F.x += gRandom->Gaus() * env_sim.si_de_x; h.R_2_F.y += +de_y_R;
		}

		// pitch error
		if (b & sbPitch)
		{
			h.L_2_F.x += gRandom->Gaus() * env_sim.si_de_P_L; h.L_2_F.y += gRandom->Gaus() * env_sim.si_de_P_L;
			h.L_1_F.x += gRandom->Gaus() * env_sim.si_de_P_L; h.L_1_F.y += gRandom->Gaus() * env_sim.si_de_P_L;
			h.R_1_F.x += gRandom->Gaus() * env_sim.si_de_P_R; h.R_1_F.y += gRandom->Gaus() * env_sim.si_de_P_R;
			h.R_2_F.x += gRandom->Gaus() * env_sim.si_de_P_R; h.R_2_F.y += gRandom->Gaus() * env_sim.si_de_P_R;
		}

		// reconstruction
		Environment env_rec = env_nom;
		Kinematics k_re = f_reco(h, env_rec);

		// extract effect
		double q_tr=0., q_re=0.;
		if (q == rqTh_x_R) { q_tr = k_tr.th_x_R; q_re = k_re.th_x_R; }
		if (q == rqTh_x_L) { q_tr = k_tr.th_x_L; q_re = k_re.th_x_L; }
		if (q == rqTh_x) { q_tr = k_tr.th_x; q_re = k_re.th_x; }
		if (q == rqTh_x_RLdiff) { q_tr = k_tr.th_x_R - k_tr.th_x_L; q_re = k_re.th_x_R - k_re.th_x_L; }
		if (q == rqTh_x_LFdiff) { q_tr = k_tr.th_x_L - k_tr.th_x; q_re = k_re.th_x_L - k_re.th_x; }
		if (q == rqTh_x_RFdiff) { q_tr = k_tr.th_x_R - k_tr.th_x; q_re = k_re.th_x_R - k_re.th_x; }
		if (q == rqTh_x_LtoRratio) { q_tr = k_tr.th_x_L / k_tr.th_x_R; q_re = k_re.th_x_L / k_re.th_x_R; }

		if (q == rqVtx_x_R) { q_tr = k_sm.vtx_x; q_re = k_re.vtx_x_R; }
		if (q == rqVtx_x_L) { q_tr = k_sm.vtx_x; q_re = k_re.vtx_x_L; }
		if (q == rqVtx_x) { q_tr = k_sm.vtx_x; q_re = k_re.vtx_x; }
		if (q == rqVtx_x_RLdiff) { q_tr = 0.; q_re = k_re.vtx_x_R - k_re.vtx_x_L; }

		if (q == rqTh_y_R) { q_tr = k_tr.th_y_R; q_re = k_re.th_y_R; }
		if (q == rqTh_y_L) { q_tr = k_tr.th_y_L; q_re = k_re.th_y_L; }
		if (q == rqTh_y) { q_tr = k_tr.th_y; q_re = k_re.th_y; }
		if (q == rqTh_y_RLdiff) { q_tr = k_tr.th_y_R - k_tr.th_y_L; q_re = k_re.th_y_R - k_re.th_y_L; }
		if (q == rqTh_y_LtoRratio) { q_tr = k_tr.th_y_L / k_tr.th_y_R; q_re = k_re.th_y_L / k_re.th_y_R; }

		double pq = q_re - q_tr;
		h_eff->Fill(pq);
	}

	string unit;
	double scale = 1.;
	if (q >= rqTh_x) { unit = "urad"; scale = 1E6; }
	if (q >= rqVtx_x) { unit = "um"; scale = 1E3; }
	if (q >= rqTh_y) { unit = "urad"; scale = 1E6; }

	mean = h_eff->GetMean() * scale;
	stddev = h_eff->GetRMS() * scale;

	delete h_eff;
}

//---------------------------------------------------------------------------------------------------

void TestOneMode(RecoQuantity q, Func f_reco, unsigned int b)
{
	// make subdirectory
	TDirectory *baseDir = gDirectory;
	gDirectory = baseDir->mkdir(SimuCodeToString(b).c_str());

	TGraph *g_mean = new TGraph(); g_mean->SetName("g_mean");
	TGraph *g_stddev = new TGraph(); g_stddev->SetName("g_stddev");
	
	unsigned int independentQuantity = 0;
	double iq_min=0., iq_max=0., iq_step=0.;
	if (q >= rqTh_x) { independentQuantity = 1; iq_min = -20E-6, iq_max = 100E-6, iq_step = 4.*5E-6; }	// rad
	if (q >= rqVtx_x) { independentQuantity = 2; iq_min = -0.30, iq_max = 0.300, iq_step = 4.*0.015; }	// mm
	if (q >= rqTh_y) { independentQuantity = 3; iq_min = -20E-6, iq_max = 100E-6, iq_step = 4.*5E-6; }	// rad


	for (double iq = iq_min; iq <= iq_max; iq += iq_step)
	{
		// central values
		Kinematics k_cv;
		k_cv.th_x = k_cv.th_x_L = k_cv.th_x_R = 0.;
		k_cv.th_y = k_cv.th_y_L = k_cv.th_y_R = 0.;
		k_cv.vtx_x = k_cv.vtx_y = 0.;

		if (independentQuantity == 1)
			k_cv.th_x = k_cv.th_x_L = k_cv.th_x_R = iq;
		if (independentQuantity == 2)
			k_cv.vtx_x = k_cv.vtx_y = iq;
		if (independentQuantity == 3)
			k_cv.th_y = k_cv.th_y_L = k_cv.th_y_R = iq;

		double mean=0., stddev=0.;

		TestOnePoint(q, f_reco, b, k_cv, mean=0., stddev=0.);
		g_mean->SetPoint(g_mean->GetN(), iq, mean);
		g_stddev->SetPoint(g_stddev->GetN(), iq, stddev);
	}
	
	g_mean->Write();
	g_stddev->Write();

	// go back to parent directory
	gDirectory = baseDir;
}

//----------------------------------------------------------------------------------------------------

void Test(const string &label, RecoQuantity q, Func f)
{
	printf("\n>> %s\n", label.c_str());

	TDirectory *baseDir = gDirectory;
	gDirectory = baseDir->mkdir(label.c_str());
	
	unsigned int bComplementary = 0;
	if (q >= rqTh_x) { bComplementary = sbVertex; }
	if (q >= rqVtx_x) { bComplementary = sbAngle; }
	if (q >= rqTh_y) { bComplementary = sbVertex; }

	TestOneMode(q, f, sbPitch);
	TestOneMode(q, f, sbBeamDivergence);
	TestOneMode(q, f, bComplementary);
	TestOneMode(q, f, sbPitch | sbBeamDivergence | bComplementary);
	//TestOneMode(q, f, sbMisalignment);
	//TestOneMode(q, f, sbOptics);
	//TestOneMode(q, f, bComplementary | sbOptics);
	//TestOneMode(q, f, sbPitch | sbBeamDivergence | bComplementary | sbOptics);
	
	gDirectory = baseDir;
}

//----------------------------------------------------------------------------------------------------

int main()
{
	gRandom->SetSeed(1);

	TFile *f_out = new TFile("test_formulae_graph.root", "recreate");

	// nominal environment
	env_nom.InitNominal();

	env_nom.si_de_P_L = 11.8E-3; env_nom.si_de_P_R = 11.8E-3;	// mm	(45 bottom - 56 top)
	//env_nom.si_de_P_L = 11.0E-3; env_nom.si_de_P_R = 11.0E-3;	// mm	(45 top - 56 bottom)
	env_nom.Print();

	// run tests
	printf("\n");
	printf("==================== theta_x, one pot ====================\n");
	Test("theta_x_one_pot_hit_L2F, th_x_L", rqTh_x_L, theta_x_one_pot_hit_L2F);
	Test("theta_x_one_pot_hit_L1F, th_x_L", rqTh_x_L, theta_x_one_pot_hit_L1F);
	//Test("theta_x_one_pot_hit_R1F, th_x_R", rqTh_x_R, theta_x_one_pot_hit_R1F);
	//Test("theta_x_one_pot_hit_R2F, th_x_R", rqTh_x_R, theta_x_one_pot_hit_R2F);

	printf("\n");
	printf("==================== theta_x, one arm ====================\n");
	Test("theta_x_one_arm_hit, th_x_L", rqTh_x_L, theta_x_one_arm_hit);
	//Test("theta_x_one_arm_hit, th_x_R", rqTh_x_R, theta_x_one_arm_hit);
	Test("theta_x_one_arm_angle, th_x_L", rqTh_x_L, theta_x_one_arm_angle);
	//Test("theta_x_one_arm_angle, th_x_R", rqTh_x_R, theta_x_one_arm_angle);
	Test("theta_x_one_arm_regr, th_x_L", rqTh_x_L, theta_x_one_arm_regr);
	//Test("theta_x_one_arm_regr, th_x_R", rqTh_x_R, theta_x_one_arm_regr);
	
	printf("\n");
	printf("==================== theta_x, two arm ====================\n");
	Test("theta_x_two_arm_hit_2F_LRavg, th_x", rqTh_x, theta_x_two_arm_hit_2F_LRavg);
	Test("theta_x_two_arm_hit_LRavg, th_x", rqTh_x, theta_x_two_arm_hit_LRavg);
	Test("theta_x_two_arm_angle_LRavg, th_x", rqTh_x, theta_x_two_arm_angle_LRavg);
	Test("theta_x_one_arm_regr_LRavg, th_x", rqTh_x, theta_x_one_arm_regr_LRavg);
	Test("theta_x_two_arm_full_regr, th_x", rqTh_x, theta_x_two_arm_full_regr);

	printf("\n");
	printf("==================== vtx_x, one arm ====================\n");
	Test("vtx_x_one_arm_regr, vtx_x_L", rqVtx_x_L, vtx_x_one_arm_regr);

	printf("\n");
	printf("==================== vtx_x, two arm ====================\n");
	Test("vtx_x_one_arm_regr_LRavg, vtx_x", rqVtx_x, vtx_x_one_arm_regr_LRavg);
	Test("vtx_x_two_arm_full_regr, vtx_x", rqVtx_x, vtx_x_two_arm_full_regr);

	printf("\n");
	printf("==================== theta_y, one pot ====================\n");
	Test("theta_y_one_pot_hit_L2F, th_y_L", rqTh_y_L, theta_y_one_pot_hit_L2F);
	Test("theta_y_one_pot_hit_L1F, th_y_L", rqTh_y_L, theta_y_one_pot_hit_L1F);
	//Test("theta_y_one_pot_hit_R1F, th_y_R", rqTh_y_R, theta_y_one_pot_hit_R1F);
	//Test("theta_y_one_pot_hit_R2F, th_y_R", rqTh_y_R, theta_y_one_pot_hit_R2F);
	
	printf("\n");
	printf("==================== theta_y, one arm ====================\n");
	Test("theta_y_one_arm_hit, th_y_L", rqTh_y_L, theta_y_one_arm_hit);
	//Test("theta_y_one_arm_hit, th_y_R", rqTh_y_R, theta_y_one_arm_hit);
	Test("theta_y_one_arm_angle, th_y_L", rqTh_y_L, theta_y_one_arm_angle);
	//Test("theta_y_one_arm_angle, th_y_R", rqTh_y_R, theta_y_one_arm_angle);
	Test("theta_y_one_arm_regr, th_y_L", rqTh_y_L, theta_y_one_arm_regr);
	//Test("theta_y_one_arm_regr, th_y_R", rqTh_y_R, theta_y_one_arm_regr);
	
	printf("\n");
	printf("==================== theta_y, two arm ====================\n");
	Test("theta_y_two_arm_hit_2F_LRavg, th_y", rqTh_y, theta_y_two_arm_hit_2F_LRavg);
	Test("theta_y_two_arm_hit_LRavg, th_y", rqTh_y, theta_y_two_arm_hit_LRavg);
	Test("theta_y_two_arm_angle_LRavg, th_y", rqTh_y, theta_y_two_arm_angle_LRavg);
	Test("theta_y_two_arm_simple_regr, th_y", rqTh_y, theta_y_two_arm_simple_regr);

	// print statistics
	printf("\n");
	printf("==================== statistics ====================\n");
	printf("\n* optics perturbations\n");
	st_opt.PrintMeanAndStdDev();
	st_opt.PrintCorrelation();

	delete f_out;
}
