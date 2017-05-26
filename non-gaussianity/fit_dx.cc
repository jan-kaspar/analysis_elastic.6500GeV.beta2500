#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"

void fit_dx()
{
	TFile *f_in = TFile::Open("../DS-fill5317/distributions_45b_56t.root");
	TH1D *h_in = (TH1D *) f_in->Get("selected - angles/period18/h_th_x_LRdiff");

	//h_in->Fit("gaus");

	/*
	TF1 *ff = new TF1("ff", "[0] * exp(- (x-[1])^2 / 2 / [2]^2)  +  [3] * exp(- (x-[4])^2 / 2 / [5]^2)");
	ff->SetParameters(4.6E9, -0.6E-06, 16E-6, -0.7E9, 0., 23E-6);
	h_in->Fit(ff, "", "", -40E-6, +40E-6);
	*/

	TF1 *ff = new TF1("ff", "[0] * exp(- (x-[1])^2 / 2 / [2]^2 + [3] * pow(x-[4], 4))");
	ff->SetParameters(3.8E9, -0.6E-6, 16.8E-6, -5.9E17, -0.7E-6);
	h_in->Fit(ff, "", "", -50E-6, +50E-6);

	TFile *f_out = TFile::Open("fit_dx.root", "recreate");
	h_in->Write("h_in");
	ff->Write("ff");
	delete f_out;
}
