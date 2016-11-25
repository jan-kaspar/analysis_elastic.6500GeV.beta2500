#include "TFile.h"
#include "TGraph2D.h"

#include "../NumericalIntegration.h"

//----------------------------------------------------------------------------------------------------

double SQRT_2_PI = sqrt(2. * M_PI);

double si_de_th_x = 0.;
double si_de_th_y = 0.;

unsigned long integ_workspace_size_de_th_x, integ_workspace_size_de_th_y;
gsl_integration_workspace *integ_workspace_de_th_x, *integ_workspace_de_th_y;

//----------------------------------------------------------------------------------------------------

double g_x(double de_th_x)
{
	double r = de_th_x / si_de_th_x;
	return exp(-r*r/2.) / SQRT_2_PI / si_de_th_x;
}

//----------------------------------------------------------------------------------------------------

double g_y(double de_th_y)
{
	double r = de_th_y / si_de_th_y;
	return exp(-r*r/2.) / SQRT_2_PI / si_de_th_y;
}

//----------------------------------------------------------------------------------------------------

double Condition(double th_x_p, double de_th_x, double th_y_p, double de_th_y)
{
	double th_x_p_R = th_x_p + de_th_x/2.;
	double th_x_p_L = th_x_p - de_th_x/2.;

	double th_y_p_R = th_y_p + de_th_y/2.;
	double th_y_p_L = th_y_p - de_th_y/2.;

	double lim_l_L = 5E-6 + ((th_x_p_L > 0.) ? 0.06 * th_x_p_L : 0.);
	double lim_h_L = 100E-6;

	double lim_l_R = 4E-6 + ((th_x_p_R > 0.) ? 0.07 * th_x_p_R : 0.);
	double lim_h_R = 100E-6;

	if (th_y_p_L < lim_l_L || th_y_p_L > lim_h_L)
		return 0.;

	if (th_y_p_R < lim_l_R || th_y_p_R > lim_h_R)
		return 0.;

	return 1.;
}

//----------------------------------------------------------------------------------------------------

double IntegOverDeThY(double x, double *par, const void* /*obj*/)
{
	double &de_th_y = x;

	double &th_x_p = par[0];
	double &th_y_p = par[1];
	double &de_th_x = par[2];

	return g_y(de_th_y) * Condition(th_x_p, de_th_x, th_y_p, de_th_y);
}

//----------------------------------------------------------------------------------------------------

double IntegOverDeThX(double x, double *par, const void* /*obj*/)
{
	double &de_th_x = x;

	double &th_x_p = par[0];
	double &th_y_p = par[1];

	double ppar[3] = { th_x_p, th_y_p, de_th_x };

	double I = RealIntegrate(IntegOverDeThY, ppar, NULL, -4.*si_de_th_y, +4.*si_de_th_y, 0., 1E-3,
		integ_workspace_size_de_th_y, integ_workspace_de_th_y, "IntegOverDeThY");

	return g_x(de_th_x) * I;
}

//----------------------------------------------------------------------------------------------------

double A_th_x_th_y(double th_x_p, double th_y_p)
{
	double par[2] = { th_x_p, th_y_p };

	return RealIntegrate(IntegOverDeThX, par, NULL, -4.*si_de_th_x, +4.*si_de_th_x, 0., 1E-3,
		integ_workspace_size_de_th_x, integ_workspace_de_th_x, "A_th_x_th_y");
}

//----------------------------------------------------------------------------------------------------

int main()
{
	// settings
	// TODO: correct values!
	si_de_th_x = 3E-6;
	si_de_th_y = 0.4E-6;

	// init integration engine
	integ_workspace_size_de_th_x = 1000;
	integ_workspace_de_th_x = gsl_integration_workspace_alloc(integ_workspace_size_de_th_x);

	integ_workspace_size_de_th_y = 1000;
	integ_workspace_de_th_y = gsl_integration_workspace_alloc(integ_workspace_size_de_th_y);

	// prepare output
	TFile *f_out = TFile::Open("acceptance_test.root", "recreate");

	// run sampling
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

	// cleaning
	delete f_out;

	return 0;
}
