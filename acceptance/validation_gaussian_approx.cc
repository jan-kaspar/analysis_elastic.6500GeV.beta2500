#include "TFile.h"
#include "TGraph.h"

#include "common_definitions.h"
#include "common_algorithms.h"
#include "AcceptanceCalculator.h"

#include "parameters_mc.h"

//----------------------------------------------------------------------------------------------------

AcceptanceCalculator accCalc;

//----------------------------------------------------------------------------------------------------

void RunOne()
{
	// run sampling
	vector<double> th_x_values = {
		-50E-6,
		0E-6,
		+20E-6,
	};

	for (const auto &th_x : th_x_values)
	{
		char buf[100];

		TGraph *g_with_app = new TGraph();
		sprintf(buf, "g_with_app_A_th_x_%+.1E", th_x);
		g_with_app->SetName(buf);
		g_with_app->SetTitle(";th_y");

		TGraph *g_without_app = new TGraph();
		sprintf(buf, "g_without_app_A_th_x_%+.1E", th_x);
		g_without_app->SetName(buf);
		g_without_app->SetTitle(";th_y");

		for (double th_y_abs = 0E-6; th_y_abs <= 10E-6; th_y_abs += 0.001E-6)
		{
			double th_y = th_y_sign * th_y_abs;

			Kinematics k;
			k.th_x = k.th_x_L = k.th_x_R = th_x;
			k.th_y = k.th_y_L = k.th_y_R = th_y;
			k.th = sqrt(k.th_x * k.th_x + k.th_y * k.th_y);

			{
				accCalc.gaussianOptimisation = true;

				double phi_corr, div_corr;
				bool skip = accCalc.Calculate(k, phi_corr, div_corr);
				double A = (skip) ? 0. : 1./div_corr;
	
				int idx = g_without_app->GetN();
				g_with_app->SetPoint(idx, th_y, A);
			}

			{
				accCalc.gaussianOptimisation = false;

				double phi_corr, div_corr;
				bool skip = accCalc.Calculate(k, phi_corr, div_corr);
				double A = (skip) ? 0. : 1./div_corr;
	
				int idx = g_without_app->GetN();
				g_without_app->SetPoint(idx, th_y, A);
			}
		}

		g_with_app->Write();
		g_without_app->Write();
	}

	TGraph *g_A_t_with_app = new TGraph();
	g_A_t_with_app->SetName("g_A_t_with_app");

	TGraph *g_A_t_without_app = new TGraph();
	g_A_t_without_app->SetName("g_A_t_without_app");

	for (double t = 0; t <= 1.0; t += 0.001)
	{
		Kinematics k;
		k.th = sqrt(t) / env.p;
		k.th_y_L = k.th_y_R = k.th_y = th_y_sign * k.th;

		accCalc.gaussianOptimisation = true;
		double phi_corr_with_app=0., div_corr_with_app=0.;
		bool skip_with_app = accCalc.Calculate(k, phi_corr_with_app, div_corr_with_app);
		double A_with_app = (skip_with_app) ? 0. : 1./phi_corr_with_app;
		g_A_t_with_app->SetPoint(g_A_t_with_app->GetN(), t, A_with_app);

		accCalc.gaussianOptimisation = false;
		double phi_corr_without_app=0., div_corr_without_app=0.;
		bool skip_without_app = accCalc.Calculate(k, phi_corr_without_app, div_corr_without_app);
		double A_without_app = (skip_without_app) ? 0. : 1./phi_corr_without_app;
		g_A_t_without_app->SetPoint(g_A_t_without_app->GetN(), t, A_without_app);
	}

	g_A_t_with_app->Write();
	g_A_t_without_app->Write();
}

//----------------------------------------------------------------------------------------------------

int main()
{
	// prepare output
	TFile *f_out = TFile::Open("validation_gaussian_approx.root", "recreate");

	// run for each diagonal
	for (unsigned int di : {0, 1})
	{
		// init diagonal
		Init_base();

		string tag_dgn;

		if (di == 0)
		{
			th_y_sign = +1;
			Init_45b_56t();
			tag_dgn = "45b_56t";
		}

		if (di == 1)
		{
			th_y_sign = -1;
			Init_45t_56b();
			tag_dgn = "45t_56b";
		}

		printf("* %s\n", tag_dgn.c_str());

		// initialise acceptance calculation
		accCalc.Init(th_y_sign, anal);
	
		// make subdirectory
		gDirectory = f_out->mkdir(tag_dgn.c_str());

		// run validation
		RunOne();
	}

	// cleaning
	delete f_out;

	return 0;
}
