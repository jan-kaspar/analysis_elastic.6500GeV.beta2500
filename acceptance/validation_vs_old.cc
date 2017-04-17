#include "TFile.h"
#include "TGraph.h"

#include "common_definitions.h"
#include "common_algorithms.h"
#include "AcceptanceCalculator.h"

#include "parameters_old.h"

//----------------------------------------------------------------------------------------------------

int main()
{
	// load settings from parameters.h
	Init_base();

	double th_y_sign = +1.;
	Init_45b_56t();

	// initialise environments
	printf("-------------------- env --------------------\n");
	env.Print();

	// initialise analysis objects
	printf("-------------------- anal --------------------\n");
	anal.Print();
	printf("\n");

	// initialise acceptance calculation
	AcceptanceCalculator accCalc;
	accCalc.Init(th_y_sign, anal);

	// prepare output
	TFile *f_out = TFile::Open("validation_vs_old.root", "recreate");

	// run sampling
	vector<double> th_x_values = {
		-50E-6,
		0E-6,
		+50E-6,
	};

	for (const auto &th_x : th_x_values)
	{
		char buf[100];

		TGraph *g_old = new TGraph();
		sprintf(buf, "g_old_A_th_x_%+.1E", th_x);
		g_old->SetName(buf);
		g_old->SetTitle(";th_y");

		TGraph *g_new = new TGraph();
		sprintf(buf, "g_new_A_th_x_%+.1E", th_x);
		g_new->SetName(buf);
		g_new->SetTitle(";th_y");

		for (double th_y = 0E-6; th_y <= 10E-6; th_y += 0.001E-6)
		{
			Kinematics k;
			k.th_x = k.th_x_L = k.th_x_R = th_x;
			k.th_y = k.th_y_L = k.th_y_R = th_y;
			k.th = sqrt(k.th_x * k.th_x + k.th_y * k.th_y);

			{
				double phi_corr, div_corr;
				bool skip = CalculateAcceptanceCorrections(th_y_sign, k, anal, phi_corr, div_corr);
				//printf("%E, %E | %i %E\n", th_x, th_y, skip, div_corr);
				double A = (skip) ? 0. : 1./div_corr;
	
				int idx = g_old->GetN();
				g_old->SetPoint(idx, th_y, A);
			}

			{
				double phi_corr, div_corr;
				bool skip = accCalc.Calculate(k, phi_corr, div_corr);
				//printf("%E, %E | %i %E\n", th_x, th_y, skip, div_corr);
				double A = (skip) ? 0. : 1./div_corr;
	
				int idx = g_new->GetN();
				g_new->SetPoint(idx, th_y, A);
			}
		}

		g_old->Write();
		g_new->Write();
	}

	/*
	TGraph2D *g2_A_th_x_th_y = new TGraph2D;
	g2_A_th_x_th_y->SetName("g2_A_th_x_th_y");
	g2_A_th_x_th_y->SetTitle(";theta_x;theta_y");

	for (double th_x = -150E-6; th_x <= 150E-6; th_x += 10E-6)
	{
		for (double th_y = 0E-6; th_y <= 15E-6; th_y += 0.1E-6)
		{
			double A = A_th_x_th_y(th_x, th_y);

			int idx = g2_A_th_x_th_y->GetN();
			g2_A_th_x_th_y->SetPoint(idx, th_x, th_y, A);
		}
	}

	g2_A_th_x_th_y->Write();
	*/

	TGraph *g_A_t_old = new TGraph();
	g_A_t_old->SetName("g_A_t_old");

	TGraph *g_A_t_new = new TGraph();
	g_A_t_new->SetName("g_A_t_new");

	for (double t = 0; t <= 1.0; t += 0.001)
	{
		Kinematics k;
		k.th = sqrt(t) / env.p;
		k.th_y_L = k.th_y_R = k.th_y = k.th;

		double phi_corr_old=0., div_corr_old=0.;
		bool skip_old = CalculateAcceptanceCorrections(th_y_sign, k, anal, phi_corr_old, div_corr_old);
		double A_old = (skip_old) ? 0. : 1./phi_corr_old;
		g_A_t_old->SetPoint(g_A_t_old->GetN(), t, A_old);

		double phi_corr_new=0., div_corr_new=0.;
		bool skip_new = accCalc.Calculate(k, phi_corr_new, div_corr_new);
		double A_new = (skip_new) ? 0. : 1./phi_corr_new;
		g_A_t_new->SetPoint(g_A_t_new->GetN(), t, A_new);
	}

	g_A_t_old->Write();
	g_A_t_new->Write();

	// cleaning
	delete f_out;

	return 0;
}
