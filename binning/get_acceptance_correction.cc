#include "TFile.h"
#include "TProfile.h"

int main()
{
	TFile *f_in = TFile::Open("../DS-fill5317/distributions_45b_56t.root");
	TProfile *acc_corr = (TProfile *) f_in->Get("acceptance correction/ob-1-30-0.05/p_t_full_corr");

	TFile *f_out = TFile::Open("acceptance_correction.root", "recreate");
	acc_corr->Write();

	delete f_out;
	delete f_in;
}
