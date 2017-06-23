#include "TFile.h"
#include "TGraph.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TF1.h"

#include <string>
#include <vector>
#include <cmath>

using namespace std;

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

TF1 *ff;

//----------------------------------------------------------------------------------------------------

void ProcessOne(TGraph *g, double rho)
{
	// build profile
	const double C = cos(rho), S = sin(rho);

	TH2D *h2 = new TH2D("", ";h;v", 100, 0., 0., 100, 0., 0.);
	TProfile *p = new TProfile("", ";h;v", 100, 0., 0.);

	for (int i = 0; i < g->GetN(); i++)
	{
		double q1, q2;
		g->GetPoint(i, q2, q1);

		double h = C*q1 + S*q2;
		double v = -S*q1 + C*q2;

		h2->Fill(h, v);
		p->Fill(h, v);
	}

	// mask bins with too low statistics
	SuppressLowStatisticsBins(p, 5);

	// determine fit range
	double x_min = 1E100, x_max = -1E100;
	for (int bi = 1; bi <= p->GetNbinsX(); bi++)
	{
		double x = p->GetBinCenter(bi);
		double v = p->GetBinContent(bi);

		if (fabs(v) > 0)
		{
			if (x_min > x)
				x_min = x;
			if (x > x_max)
				x_max = x;
		}
	}

	//printf("    x_min = %.3f, x_max = %.3f\n", x_min, x_max);

	double x_min_fit = x_min + (x_max - x_min) * 0.1;
	double x_max_fit = x_max - (x_max - x_min) * 0.1;

	//printf("    x_min_fit = %.3f, x_max_fit = %.3f\n", x_min_fit, x_max_fit);

	// make fit
	p->Fit(ff, "Q", "", x_min_fit, x_max_fit);

	// save plots
	h2->Write("h2");
	p->Write("p");

	// determine value of constriant
	const double al = ff->GetParameter(1);
	const double al_unc = ff->GetParError(1);

	const double a = (C - S*al) / (C*al + S);
	const double a_unc = al_unc / (C*al + S) / (C*al + S);

	printf("                al = %.4E, a = %.4E +- %.4E\n", al, a, a_unc);

	// save data
	TGraph *g_data = new TGraph();
	g_data->SetPoint(0, al, al_unc);
	g_data->SetPoint(1, a, a_unc);
	g_data->Write("g_data");
}

//----------------------------------------------------------------------------------------------------

int main()
{
	// input configuration
	vector<string> datasets;
	datasets.push_back("DS-fill5313");
	datasets.push_back("DS-fill5314");
	datasets.push_back("DS-fill5317");
	datasets.push_back("DS-fill5321");

	vector<string> diagonals;
	diagonals.push_back("45b_56t");
	diagonals.push_back("45t_56b");

	// constraint configuration
	struct ConstraintInfo
	{
		string tag, input;
		vector<double> rotations;
	};

	vector<ConstraintInfo> constraintInfo = {
		{ "R1", "vtx_x_0.1sigma/g_thl_x_R_vs_thl_x_L", { 0., M_PI/2., -M_PI/4. } },
		{ "R2", "vtx_x_0.1sigma/g_y_R_220_F_vs_y_L_220_F", { 0., M_PI/2., -M_PI/4. } },

		{ "R3", "vtx_x_0.1sigma/g_thl_y_R_vs_y_R_220_F", { 0., M_PI/2., 1.57078 } },
		{ "R4", "vtx_x_0.1sigma/g_thl_y_L_vs_y_L_220_F", { 0., M_PI/2., 1.57078 } },
                                                     
		{ "R5", "vtx_x_0.1sigma/g_thl_x_R_vs_x_R_220_F", { 0., M_PI/2., -1.57073 } },
		{ "R6", "vtx_x_0.1sigma/g_thl_x_L_vs_x_L_220_F", { 0., M_PI/2., -1.57073 } },

		{ "R7", "vtx_x_0.1sigma/g_x_L_220_F_vs_y_L_220_F", { M_PI/2. } },
		{ "R8", "vtx_x_0.1sigma/g_x_L_210_F_vs_y_L_210_F", { M_PI/2. } },
		{ "R9", "vtx_x_0.1sigma/g_x_R_210_F_vs_y_R_210_F", { M_PI/2. } },
		{ "R10", "vtx_x_0.1sigma/g_x_R_220_F_vs_y_R_220_F", { M_PI/2. } },
	};

	// initialisation
	ff = new TF1("ff", "1++x");

	// prepare output (validation)
	TFile *f_out = TFile::Open("make_fits.root", "recreate");
	
	// process data
	for (unsigned int dsi = 0; dsi < datasets.size(); dsi++)
	{
		printf("\n\n%s\n", datasets[dsi].c_str());

		TDirectory *d_dataset = f_out->mkdir(datasets[dsi].c_str());

		for (unsigned int dgni = 0; dgni < diagonals.size(); dgni++)
		{
			printf("    %s\n", diagonals[dgni].c_str());

			TDirectory *d_diagonal = d_dataset->mkdir(diagonals[dgni].c_str());

			for (const auto constraint : constraintInfo)
			{
				printf("        %s\n", constraint.tag.c_str());
				TDirectory *d_constraint = d_diagonal->mkdir(constraint.tag.c_str());

				string f_in_name = "../"+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root";
				TFile *f_in = TFile::Open(f_in_name.c_str());
				if (!f_in)
				{
					printf("ERROR: file '%s' can not be opened.\n", f_in_name.c_str());
					continue;
				}

				string g_in_path = "optics/matching input, " + constraint.input;
				TGraph *g_in = (TGraph *) f_in->Get(g_in_path.c_str());
				if (!g_in)
				{
					printf("ERROR: object '%s' can not be loaded.\n", g_in_path.c_str());
					continue;
				}

				for (double rotation : constraint.rotations)
				{
					printf("            %.5f\n", rotation);
				
					char buf[100];
					sprintf(buf, "%.5f", rotation);
					gDirectory = d_constraint->mkdir(buf);

					ProcessOne(g_in, rotation);
				}

				delete f_in;
			}
		}
	}

	// clean up
	delete f_out;

	return 0;
}
