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

using namespace std;

#include "formulae.h"

#include "common.h"


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
		// run simulation
		Kinematics k_tr, k_sm, k_re;
		SimulateEvent(k_cv, b, f_reco, k_tr, k_sm, k_re);

		// fill plots
		double e = ExtractEffect(q, k_tr, k_sm, k_re);
		h_eff->Fill(e);
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
	TestOneMode(q, f, sbOptics);
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
	env_nom.UseMatchedOptics();

	// case 1
	/*
	env_nom.si_vtx_x = 480E-3;							// mm
	env_nom.si_th_x_L = env_nom.si_th_x_R = 0.27E-6;	// rad
	env_nom.si_de_P_L = env_nom.si_de_P_R = 11E-3;		// mm
	*/

	// case 2
	env_nom.si_vtx_x = 680E-3;							// mm
	env_nom.si_th_x_L = env_nom.si_th_x_R = 0.39E-6;	// rad
	env_nom.si_de_P_L = env_nom.si_de_P_R = 12E-3;		// mm

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
	st_opt.PrintStat();
	st_opt.PrintMeanAndStdDev();
	printf("correlation matrix\n");
	st_opt.PrintCorrelation();

	delete f_out;
}
