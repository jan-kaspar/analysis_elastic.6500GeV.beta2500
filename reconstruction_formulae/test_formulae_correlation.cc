#include "TRandom2.h"
#include "TH1D.h"
#include "TH2D.h"
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

void TestOnePoint(RecoQuantity q1, RecoQuantity q2, Func f_reco, unsigned int b, const Kinematics k_cv)
{
	// prepare output
	TGraph *g_scatter = new TGraph();
	g_scatter->SetName("g_scatter");

	TH2D *h2_scatter = new TH2D("h2_scatter", "", 100, 0., 0., 100, 0., 0.);

	// event loop
	unsigned int N_ev = 100000;
	for (unsigned int ev = 0; ev < N_ev; ev++)
	{
		// run simulation
		Kinematics k_tr, k_sm, k_re;
		SimulateEvent(k_cv, b, f_reco, k_tr, k_sm, k_re);

		// extract effects
		double e1 = ExtractEffect(q1, k_tr, k_sm, k_re);
		double e2 = ExtractEffect(q2, k_tr, k_sm, k_re);

		// fill plots
		int idx = g_scatter->GetN();
		if (idx < 1000)
			g_scatter->SetPoint(idx, e1, e2);

		h2_scatter->Fill(e1, e2);
	}

	// save output
	g_scatter->Write();
	h2_scatter->Write();
}

//---------------------------------------------------------------------------------------------------

void TestOneMode(RecoQuantity q1, RecoQuantity q2, Func f_reco, unsigned int b)
{
	// make subdirectory
	TDirectory *baseDir = gDirectory;
	TDirectory *d_mode = baseDir->mkdir(SimuCodeToString(b).c_str());

	// list of central points
	vector<Kinematics> k_cvs;

	Kinematics k_cvp;

	k_cvp.th_x = k_cvp.th_x_L = k_cvp.th_x_R = 50E-6;	// rad
	k_cvp.th_y = k_cvp.th_y_L = k_cvp.th_y_R = 50E-6;	// rad
	k_cvp.vtx_x = k_cvp.vtx_y = 0.;	// mm
	k_cvs.push_back(k_cvp);

	k_cvp.th_x = k_cvp.th_x_L = k_cvp.th_x_R = 5E-6;	// rad
	k_cvp.th_y = k_cvp.th_y_L = k_cvp.th_y_R = 5E-6;	// rad
	k_cvp.vtx_x = k_cvp.vtx_y = 0.;	// mm
	k_cvs.push_back(k_cvp);

	// loop over centra values
	for (const auto &k_cv : k_cvs)
	{
		// make subdirectory
		char buf[100];
		sprintf(buf, "th_x=%.2E,th_y=%.2E,vtx_x=%.2E", k_cv.th_x, k_cv.th_y, k_cv.vtx_x);
		gDirectory = d_mode->mkdir(buf);

		TestOnePoint(q1, q2, f_reco, b, k_cv);
	}
	
	// go back to parent directory
	gDirectory = baseDir;
}

//----------------------------------------------------------------------------------------------------

void Test(const string &label, RecoQuantity q1, RecoQuantity q2, Func f)
{
	printf("\n>> %s\n", label.c_str());

	TDirectory *baseDir = gDirectory;
	gDirectory = baseDir->mkdir(label.c_str());
	
	TestOneMode(q1, q2, f, sbPitch);
	TestOneMode(q1, q2, f, sbBeamDivergence);
	TestOneMode(q1, q2, f, sbVertex);
	TestOneMode(q1, q2, f, sbPitch | sbBeamDivergence | sbVertex);
	//TestOneMode(q1, q2, f, sbMisalignment);
	//TestOneMode(q1, q2, f, sbOptics);
	
	gDirectory = baseDir;
}

//----------------------------------------------------------------------------------------------------

int main()
{
	gRandom->SetSeed(1);

	TFile *f_out = new TFile("test_formulae_correlation.root", "recreate");

	// nominal environment
	env_nom.InitNominal();

	env_nom.si_de_P_L = 11.8E-3; env_nom.si_de_P_R = 11.8E-3;	// mm	(45 bottom - 56 top)
	//env_nom.si_de_P_L = 11.0E-3; env_nom.si_de_P_R = 11.0E-3;	// mm	(45 top - 56 bottom)
	env_nom.Print();

	// run tests
	printf("\n");
	printf("==================== theta_x, one pot ====================\n");
	Test("theta_x_one_pot_hit_LR2F", rqTh_x_R, rqTh_x_L, theta_x_one_pot_hit_LR2F);

	printf("\n");
	printf("==================== theta_x, one pot ====================\n");
	Test("theta_y_one_pot_hit_LR2F", rqTh_y_R, rqTh_y_L, theta_y_one_pot_hit_LR2F);

	delete f_out;
}
