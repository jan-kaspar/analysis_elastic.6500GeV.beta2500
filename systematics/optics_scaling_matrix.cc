#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TMatrixDSymEigen.h"
#include "TVectorD.h"
#include "TMath.h"

//----------------------------------------------------------------------------------------------------

void DoubleArm()
{
	double si_de_th_x = 0.165;
	double si_de_th_y = 0.152;
	double corr = -0.903;

	double V_data[] = {
		si_de_th_x*si_de_th_x, si_de_th_x*si_de_th_y * corr,
		si_de_th_x*si_de_th_y * corr, si_de_th_y*si_de_th_y
	};

	TMatrixDSym V(2);
	V.SetMatrixArray(V_data);
	V.Print();

	TMatrixDSymEigen eig_decomp(V);
	TVectorD eig_values(eig_decomp.GetEigenValues());
	TMatrixD eig_vectors = eig_decomp.GetEigenVectors(); // vectors in columns
	for (unsigned int i = 0; i < 2; i++)
	{
		printf("eigenvector %i: eigenvalue=%.3E, eigenvector=(", i, eig_values(i));

		for (unsigned int j = 0; j < 2; j++)
			printf("%.5f, ", eig_vectors(j, i));

		printf(")\n");
	}

	printf("\n* modes\n");
	TMatrixDSym S(2);
	for (unsigned int i = 0; i < 2; i++)
		S(i, i) = sqrt(eig_values(i));

	for (unsigned int i = 0; i < 2; i++)
	{
		printf("mode %u: ", i);

		TVectorD eta(2);
		eta(i) = 1.;

		TVectorD m = eig_vectors * S * eta;

		for (unsigned int j = 0; j < 2; j++)
			printf("%.3E, ", m(j));

		printf("\n");
	}
}

//----------------------------------------------------------------------------------------------------

void SingleArm()
{
	// order: th_x_L, th_y_L, th_x_R, th_y_R 

	double V_data[] = {
		 2.984e-06,  -2.251e-06,   2.444e-06,  -2.259e-06,
		-2.251e-06,   2.301e-06,  -2.278e-06,   2.309e-06,
		 2.444e-06,  -2.278e-06,   3.058e-06,  -2.284e-06,
		-2.259e-06,   2.309e-06,  -2.284e-06,   2.316e-06
	};

	TMatrixDSym V(4);
	V.SetMatrixArray(V_data);
	V.Print();

	TMatrixDSymEigen eig_decomp(V);
	TVectorD eig_values(eig_decomp.GetEigenValues());
	TMatrixD eig_vectors = eig_decomp.GetEigenVectors(); // vectors in columns
	for (unsigned int i = 0; i < 4; i++)
	{
		printf("eigenvector %i: eigenvalue=%.3E, eigenvector=(", i, eig_values(i));

		for (unsigned int j = 0; j < 4; j++)
			printf("%.5f, ", eig_vectors(j, i));

		printf(")\n");
	}

	printf("\n* modes\n");
	TMatrixDSym S(4);
	for (unsigned int i = 0; i < 4; i++)
		S(i, i) = (eig_values(i) >= 0.) ? sqrt(eig_values(i)) : 0.;

	for (unsigned int i = 0; i < 4; i++)
	{
		printf("mode %u: ", i);

		TVectorD eta(4);
		eta(i) = 1.;

		TVectorD m = eig_vectors * S * eta;

		for (unsigned int j = 0; j < 4; j++)
			printf("%+.3E, ", m(j));

		printf("\n");
	}
}

//----------------------------------------------------------------------------------------------------

int main()
{
	printf("\n--------------------------------------------------\n");
	printf("double arm\n");
	printf("--------------------------------------------------\n");
	DoubleArm();

	printf("\n--------------------------------------------------\n");
	printf("single arm\n");
	printf("--------------------------------------------------\n");
	SingleArm();
}
