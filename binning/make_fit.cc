#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"

int main()
{
	TFile *f_in = new TFile("merged.root");
	TH1D *h_in = (TH1D *) f_in->Get("ub/merged/45b_56t/h_dsdt");
	//TH1D *h_in = (TH1D *) f_in->Get("ub/merged/45t_56b/h_dsdt");

	h_in->Scale(0.9E-8);

	TF1 *ff = new TF1("ff", "[0]*exp(-[1]*x+[2]*x*x+[3]*x*x*x+[4]*x*x*x*x+[5]*x*x*x*x*x)+TMath::Erf((x-[6])/[7])*[8]*exp(-[9]*x)", 0., 3.);

	TCanvas *c = new TCanvas();
	c->SetLogy(1);

	ff->SetParameter(0, 6.5E2);
	ff->SetParameter(1, 20.5);
	ff->SetParameter(2, 7.1);
	ff->SetParameter(3, -11.);
	ff->FixParameter(4, 0.);
	ff->FixParameter(5, 0.);
	
	ff->SetParameter(6, 0.5);
	ff->SetParameter(7, 0.2);
	
	ff->SetParameter(8, 2.43E0);
	ff->SetParameter(9, 5.);

	h_in->Fit(ff, "", "", 0.036, 3.0);

	printf("ff(0) = %f mb / GeV^2\n", ff->Eval(0.));

	TFile *f_out = new TFile("dsigma_dt_fit.root", "recreate");
	h_in->Write();
	ff->Write();

	delete f_out;

	return 0;
}
