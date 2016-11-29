#include "NumericalIntegration.h"

//----------------------------------------------------------------------------------------------------

struct AcceptanceCalculator
{
	double th_y_sign;
	Analysis anal;

	bool gaussianOptimisation;

	unsigned long integ_workspace_size_de_th_x, integ_workspace_size_de_th_y;
	gsl_integration_workspace *integ_workspace_de_th_x, *integ_workspace_de_th_y;

	void Init(double _th_y_sign, const Analysis &_anal)
	{
		th_y_sign = _th_y_sign;
		anal = _anal;

		gaussianOptimisation = true;

		integ_workspace_size_de_th_x = 1000;
		integ_workspace_de_th_x = gsl_integration_workspace_alloc(integ_workspace_size_de_th_x);

		integ_workspace_size_de_th_y = 1000;
		integ_workspace_de_th_y = gsl_integration_workspace_alloc(integ_workspace_size_de_th_y);
	}

	double dist_de_th_x(double de_th_x) const;

	double dist_de_th_y(double de_th_y) const;

	double Condition(double th_x_p, double de_th_x, double th_y_p, double de_th_y) const;

	static double IntegOverDeThY(double x, double *par, const void* obj);

	static double IntegOverDeThX(double x, double *par, const void* obj);

	double SmearingFactor(double th_x_p, double th_y_p) const;

	bool Calculate(const Kinematics &k, double &phi_corr, double &div_corr) const;
};


//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::dist_de_th_x(double de_th_x) const
{
	double si_de_th_x = anal.si_th_x_1arm_L * sqrt(2.);
	double r = de_th_x / si_de_th_x;
	return exp(-r*r/2.) / sqrt(2. * M_PI) / si_de_th_x;
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::dist_de_th_y(double de_th_y) const
{
	double si_de_th_y = anal.si_th_y_1arm * sqrt(2.);
	double r = de_th_y / si_de_th_y;
	return exp(-r*r/2.) / sqrt(2. * M_PI) / si_de_th_y;
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::Condition(double th_x_p, double de_th_x, double th_y_p, double de_th_y) const
{
	double th_x_p_R = th_x_p + de_th_x/2.;
	double th_x_p_L = th_x_p - de_th_x/2.;

	double th_y_p_R = th_y_p + de_th_y/2.;
	double th_y_p_L = th_y_p - de_th_y/2.;

	double th_y_L_cut_l = anal.fc_L_l.GetThYLimit(th_x_p_L);
	double th_y_L_cut_h = anal.fc_L_h.GetThYLimit(th_x_p_L);

	double th_y_R_cut_l = anal.fc_R_l.GetThYLimit(th_x_p_R);
	double th_y_R_cut_h = anal.fc_R_h.GetThYLimit(th_x_p_R);

	if (th_y_p_L < th_y_L_cut_l || th_y_p_L > th_y_L_cut_h)
		return 0.;

	if (th_y_p_R < th_y_R_cut_l || th_y_p_R > th_y_R_cut_h)
		return 0.;

	return 1.;
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::IntegOverDeThY(double x, double *par, const void* obj)
{
	double &de_th_y = x;

	double &th_x_p = par[0];
	double &th_y_p = par[1];
	double &de_th_x = par[2];

	AcceptanceCalculator *ac = (AcceptanceCalculator *) obj;

	return ac->dist_de_th_y(de_th_y) * ac->Condition(th_x_p, de_th_x, th_y_p, de_th_y);
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::IntegOverDeThX(double x, double *par, const void* obj)
{
	double &de_th_x = x;

	double &th_x_p = par[0];
	double &th_y_p = par[1];

	AcceptanceCalculator *ac = (AcceptanceCalculator *) obj;

	double ppar[3] = { th_x_p, th_y_p, de_th_x };

	double si_de_th_y = ac->anal.si_th_y_1arm * sqrt(2.);

	double I = 0.;
	
	if (ac->gaussianOptimisation)
	{
		double th_x_p_R = th_x_p + de_th_x/2.;
		double th_x_p_L = th_x_p - de_th_x/2.;

		double th_y_L_cut_l = ac->anal.fc_L_l.GetThYLimit(th_x_p_L);
		double th_y_L_cut_h = ac->anal.fc_L_h.GetThYLimit(th_x_p_L);

		double th_y_R_cut_l = ac->anal.fc_R_l.GetThYLimit(th_x_p_R);
		double th_y_R_cut_h = ac->anal.fc_R_h.GetThYLimit(th_x_p_R);

		double th_y_abs = ac->th_y_sign * th_y_p;

		double UB_y = min(th_y_R_cut_h - th_y_abs, th_y_abs - th_y_L_cut_l);
		double LB_y = max(th_y_R_cut_l - th_y_abs, th_y_abs - th_y_L_cut_h);
		I = (UB_y > LB_y) ? ( TMath::Erf(UB_y / ac->anal.si_th_y_1arm) - TMath::Erf(LB_y / ac->anal.si_th_y_1arm) ) / 2. : 0.;
	} else {
		I = RealIntegrate(AcceptanceCalculator::IntegOverDeThY, ppar, ac, -6.*si_de_th_y, +6.*si_de_th_y, 0., 1E-5,
			ac->integ_workspace_size_de_th_y, ac->integ_workspace_de_th_y, "AcceptanceCalculator::IntegOverDeThX");
	}

	return ac->dist_de_th_x(de_th_x) * I;
}

//----------------------------------------------------------------------------------------------------

double AcceptanceCalculator::SmearingFactor(double th_x_p, double th_y_p) const
{
	double par[2] = { th_x_p, th_y_p };

	double si_de_th_x = anal.si_th_x_1arm_L * sqrt(2.);

	return RealIntegrate(AcceptanceCalculator::IntegOverDeThX, par, this, -6.*si_de_th_x, +6.*si_de_th_x, 0., 1E-5,
		integ_workspace_size_de_th_x, integ_workspace_de_th_x, "AcceptanceCalculator::SmearingFactor");
}

//----------------------------------------------------------------------------------------------------

bool AcceptanceCalculator::Calculate(const Kinematics &k, double &phi_corr, double &div_corr) const
{
	// ---------- smearing component ----------

	double th_y_L_cut_l = anal.fc_L_l.GetThYLimit(k.th_x_L);
	double th_y_L_cut_h = anal.fc_L_h.GetThYLimit(k.th_x_L);

	double th_y_R_cut_l = anal.fc_R_l.GetThYLimit(k.th_x_R);
	double th_y_R_cut_h = anal.fc_R_h.GetThYLimit(k.th_x_R);

	if ((th_y_sign * k.th_y_L < th_y_L_cut_l) || (th_y_sign * k.th_y_R < th_y_R_cut_l)
		|| (th_y_sign * k.th_y_L > th_y_L_cut_h) || (th_y_sign * k.th_y_R > th_y_R_cut_h))
		return true;

	double F = SmearingFactor(k.th_x, k.th_y);
	div_corr = 1. / F;

	// ---------- phi component ----------

	double th_y_G_cut_l = anal.fc_G_l.GetThYLimit(k.th_x);
	double th_y_G_cut_h = anal.fc_G_h.GetThYLimit(k.th_x);

	if ((th_y_sign * k.th_y < th_y_G_cut_l) || (th_y_sign * k.th_y > th_y_G_cut_h))
		return true;

	// get all intersections
	set<double> phis;

	for (const auto &phi : anal.fc_G_l.GetIntersectionPhis(k.th))
		phis.insert(phi);

	for (const auto &phi : anal.fc_G_h.GetIntersectionPhis(k.th))
		phis.insert(phi);

	// the number of intersections must be even
	if ((phis.size() % 2) == 1)
	{
		printf("ERROR: odd number of intersections in acceptance calculation\n");
	}

	// no intersection => no acceptances
	if (phis.size() == 0)
		return true;

	// calculate arc-length in within acceptance
	double phiSum = 0.;
	for (set<double>::iterator it = phis.begin(); it != phis.end(); ++it)
	{
		double phi_start = *it;
		++it;
		double phi_end = *it;

		phiSum += phi_end - phi_start;
	}
	
	phi_corr = 2. * M_PI / phiSum;

	return false;
}
