#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"

#include <vector>

using namespace std;

//----------------------------------------------------------------------------------------------------

vector<TF1 *> parameterizations;

TF1* InitFitFunctions(double norm = 600)
{
	printf(">> InitFitFunctions(%E)\n", norm);

	TF1 *ff;

	// tried and non-working

	/*
	ff = new TF1("exp3+exp2", "[0]*exp([1]*x + [2]*x*x + [3]*x*x*x) + [4]*exp([5]*x + [6]*x*x)");
	ff->SetParameters(norm, -20., 0., 0., norm/1E3, -3., 0.);
	parameterizations.push_back(ff);

	ff = new TF1("exp3+exp3", "[0]*exp([1]*x + [2]*x*x + [3]*x*x*x) + [4]*exp([5]*x + [6]*x*x + [7]*x*x*x)");
	ff->SetParameters(norm, -20., 0., 0., norm/1E3, -3., 0., 0.);
	parameterizations.push_back(ff);

	ff = new TF1("exp3+exp4", "[0]*exp([1]*x + [2]*x*x + [3]*x*x*x) + [4]*exp([5]*x + [6]*x*x + [7]*x*x*x + [8]*x*x*x*x)");
	ff->SetParameters(norm, -20., 0., 0., norm/1E3, -3., 0., 0., 0.);
	parameterizations.push_back(ff);
	*/
	
	ff = new TF1("exp4+exp2", "[0]*exp([1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x) + [5]*exp([6]*x + [7]*x*x)");
	ff->SetParameters(norm, -20., 0., 0., 0., norm/1E3, -3., 0.);
	parameterizations.push_back(ff);

	/*
	ff = new TF1("exp5+erf*exp2", "[0] * exp(-[1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x + [5]*x*x*x*x*x) + TMath::Erf((x - [6])/[7]) * [8] * exp(-[9]*x + [10]*x*x)");
	ff->SetParameter(0, 539.);
	ff->SetParameter(1, 20.6);
	ff->SetParameter(2, 13.2);
	ff->SetParameter(3, -46.);
	ff->SetParameter(4, 63.6);
	ff->SetParameter(5, -26.2);

	ff->SetParameter(6, 0.55);
	ff->SetParameter(7, 0.3);

	ff->SetParameter(8, 1.4);
	ff->SetParameter(9, 4.8);
	ff->SetParameter(10, 0.);
	ff->SetRange(anal.t_min_full, anal.t_max_full);
	parameterizations.push_back(ff);
	*/
	
	ff = new TF1("p1*exp3+p1*exp1", "([0] + [1]*x) * exp([2]*x + [3]*x*x + [4]*x*x*x) + ([5] + [6]*x) * exp([7]*x)");
	ff->SetParameter(0, norm);
	ff->SetParameter(1, 0.);
	ff->SetParameter(2, -20.1);
	ff->SetParameter(3, 8.37);
	ff->SetParameter(4, -11.1);
	ff->SetParameter(5, -6.99);
	ff->SetParameter(6, 12.85);
	ff->SetParameter(7, -6.17);
	parameterizations.push_back(ff);
	
	ff = new TF1("p1*exp3+p2*exp2", "([0] + [1]*x) * exp([2]*x + [3]*x*x + [4]*x*x*x) + ([5] + [6]*x + [7]*x*x) * exp([8]*x + [9]*x*x)");
	ff->SetParameter(0, norm);
	ff->SetParameter(1, 0.);
	ff->SetParameter(2, -20.1);
	ff->SetParameter(3, 8.37);
	ff->SetParameter(4, -11.1);
	ff->SetParameter(5, -6.99);
	ff->SetParameter(6, 12.85);
	ff->SetParameter(7, 0.);
	ff->SetParameter(8, -6.17);
	ff->SetParameter(9, 0.);
	parameterizations.push_back(ff);

	ff = new TF1("exp3-intf-exp1", "[1]*[1]*exp(2*[2]*x + 2*[3]*x*x + 2*[4]*x*x*x) + 2 * cos([0]) * [1]*exp([2]*x + [3]*x*x + [4]*x*x*x) * [5]*exp([6]*x) + [5]*[5]*exp(2*[6]*x)");
	ff->SetParameter(0, 2.77);
	ff->SetParameter(1, 24.33);
	ff->SetParameter(2, -9.71);
	ff->SetParameter(3, 4.52);
	ff->SetParameter(4, -3.32);
	ff->SetParameter(5, 1.33);
	ff->SetParameter(6, -2.45);
	parameterizations.push_back(ff);
	
	return ff;
}

//----------------------------------------------------------------------------------------------------

int main()
{
	// settings
	InitFitFunctions();

	// get input
	TFile *f_in = new TFile("../DS-merged/merged.root");
	TH1D *h_in = (TH1D *) f_in->Get("ob-2-10-0.05/merged/combined/h_dsdt");

	// prepare output
	TFile *f_out = new TFile("make_fits.root", "recreate");

	// do fits
	for (const auto fcn : parameterizations)
	{
		printf("\n\t---------- %s ----------\n", fcn->GetName());

		gDirectory = f_out->mkdir(fcn->GetName());

		h_in->Fit(fcn, "Q", "", 0.005, 1.1);
		h_in->Fit(fcn, "", "", 0.005, 1.1);

		h_in->Write();
		fcn->Write();
	}

	// clean up
	delete f_out;

	return 0;
}
