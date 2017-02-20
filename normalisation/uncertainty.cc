#include "TFile.h"
#include "TH1D.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TGraph.h"

#include <cmath>

using namespace std;

//----------------------------------------------------------------------------------------------------

int main()
{
	// configuration
	//string binning = "ub-90m";
	string binning = "ob-3-4";
	//string binning = "ob-0-1";

	double t_min = 0.02, t_max = 0.2;


	string dataset = "DS2b";

	// get covariance matrix
	TFile *f_in = new TFile(("../systematics/" + dataset + "/matrix_numerical_integration.root").c_str());
	TMatrixD *rel_cov_mat_p = (TMatrixD *) f_in->Get(("matrices/all-anal/combined/" + binning + "/cov_mat").c_str());
	TMatrixD rel_cov_mat = TMatrixD(*rel_cov_mat_p);
	TH1D *h_unc_stddev = (TH1D *) f_in->Get(("matrices/all-anal/combined/" + binning + "/h_stddev").c_str());

	// get fit
	TFile *f_in_fit = new TFile(("../coulomb_analysis/data/" + binning + "/merged/combined/fit:3,KL,cen,chisq,,st+sy/dsdt.root").c_str());
	TH1D *h_data = (TH1D *) f_in_fit->Get("data_hist");
	TGraph *g_fit = (TGraph *) f_in_fit->Get("ff_T, final");

	// determine range
	int bin_min = h_data->FindBin(t_min);
	int bin_max = h_data->FindBin(t_max);

	printf("* bin_min = %i, bin_max = %i\n", bin_min, bin_max);

	// TODO
	int dim = bin_max - bin_min + 1;

	TMatrixDSym cov_mat(dim);
	TMatrixD P(1, dim);
	
	TVectorD s(dim);

	for (int bi = bin_min; bi <= bin_max; bi++)
	{
		int ri = bi - 1;		// rel_cov_mat index
		int mi = bi - bin_min;	// cov_mat, P and s index

		double ci = h_unc_stddev->GetBinCenter(bi);
		double wi = h_unc_stddev->GetBinWidth(bi);
		double vi = g_fit->Eval(ci);
		double si = vi * wi;

		s(mi) = si;
		P(0, mi) = 1.;

		for (int bj = bin_min; bj <= bin_max; bj++)
		{
			int rj = bj - 1;
			int mj = bj - bin_min;

			double cj = h_unc_stddev->GetBinCenter(bj);
			double wj = h_unc_stddev->GetBinWidth(bj);
			double vj = g_fit->Eval(cj);
			double sj = vj * wj;

			cov_mat(mi, mj) = si * rel_cov_mat(ri, rj) * sj;
		}

	}

	// integral
	TVectorD Iv = P * s;
	double I = Iv(0);

	// integral uncertainty
	TMatrixD PT(TMatrixD::kTransposed, P);
	TMatrixD VI = P * cov_mat * PT;
	double si_I = sqrt(VI(0, 0));

	printf("I = %E +- %E (rel. unc. %.4f)\n", I, si_I, si_I / I);

	return 0;
}
