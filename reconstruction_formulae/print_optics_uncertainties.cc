#include <string>
#include <vector>
#include <cmath>

#include "../common_definitions.h"
#include "../common_algorithms.h"

//----------------------------------------------------------------------------------------------------

TVectorD GetRelAngDiff(const Environment &env_test, const Environment &env_nom)
{
	TVectorD D(6);

	D(0) = env_test.L_x_L_2_F / env_nom.L_x_L_2_F - 1.;
	D(1) = env_test.L_y_L_2_F / env_nom.L_y_L_2_F - 1.;

	D(2) = env_test.L_x_R_2_F / env_nom.L_x_R_2_F - 1.;
	D(3) = env_test.L_y_R_2_F / env_nom.L_y_R_2_F - 1.;
	
	D(4) = (env_test.L_x_L_2_F / env_nom.L_x_L_2_F + env_test.L_x_R_2_F / env_nom.L_x_R_2_F) / 2. - 1.;
	D(5) = (env_test.L_y_L_2_F / env_nom.L_y_L_2_F + env_test.L_y_R_2_F / env_nom.L_y_R_2_F) / 2. - 1.;

	return D;
}

//----------------------------------------------------------------------------------------------------

void StudyEigenModes(unsigned int mode_start, unsigned int mode_end, const Environment &env_nom)
{
	printf("* study eigen modes %u to %u\n", mode_start, mode_end);

	TMatrixDSym VD(6);

	for (unsigned int mode = mode_start; mode <= mode_end; mode++)
	{
		Environment env_test(env_nom);
		env_test.ApplyOpticsPerturbationMode(mode, 1.);

		const TVectorD &D = GetRelAngDiff(env_test, env_nom);

		for (unsigned int i = 0; i < 6; i++)
		{
			for (unsigned int j = 0; j < 6; j++)
			{
				VD(i, j) += D(i) * D(j);
			}
		}
	}

	const double si_de_x_L = sqrt(VD(0, 0));
	const double si_de_y_L = sqrt(VD(1, 1));

	const double si_de_x_R = sqrt(VD(2, 2));
	const double si_de_y_R = sqrt(VD(3, 3));

	const double si_de_x_G = sqrt(VD(4, 4));
	const double si_de_y_G = sqrt(VD(5, 5));

	const double corr_de_x_de_y_G = VD(4, 5) / si_de_x_G / si_de_y_G;

	printf("    left arm reconstruction: si(de_x) = %.5f, si(de_y) = %.5f\n", si_de_x_L, si_de_y_L);
	printf("    right arm reconstruction: si(de_x) = %.5f, si(de_y) = %.5f\n", si_de_x_R, si_de_y_R);

	printf("    double-arm reconstruction: si(de_x) = %.5f, si(de_y) = %.5f, corr = %.2f %%\n",
		si_de_x_G, si_de_y_G, corr_de_x_de_y_G * 1E2);
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

int main()
{
	// nominal environment
	Environment env;
	env.InitNominal();
	env.UseMatchedOptics();

	env.Print();

	//----------------------------------------------------------------------------------------------------

	const double si_L_x_L_2_F = sqrt(env.opt_cov(5, 5));	// m
	const double si_L_x_R_2_F = sqrt(env.opt_cov(13, 13));

	const double si_L_y_L_2_F = sqrt(env.opt_cov(7, 7));
	const double si_L_y_R_2_F = sqrt(env.opt_cov(15, 15));

	const double corr_L_x = env.opt_cov(5, 13) / si_L_x_L_2_F / si_L_x_R_2_F;
	const double corr_L_y = env.opt_cov(7, 15) / si_L_y_L_2_F / si_L_y_R_2_F;

	printf("\n");
	printf("* L_x\n");
	printf("    L-220-fr: %.3f m --> %.3f%%\n", si_L_x_L_2_F, si_L_x_L_2_F / env.L_x_L_2_F * 1E3 * 1E2);
	printf("    R-220-fr: %.3f m --> %.3f%%\n", si_L_x_R_2_F, si_L_x_R_2_F / env.L_x_R_2_F * 1E3 * 1E2);
	printf("    L-R correlation: %.3f%%\n", corr_L_x * 1E2);

	printf("\n");
	printf("* L_y\n");
	printf("    L-220-fr: %.3f m --> %.3f%%\n", si_L_y_L_2_F, si_L_y_L_2_F / env.L_y_L_2_F * 1E3 * 1E2);
	printf("    R-220-fr: %.3f m --> %.3f%%\n", si_L_y_R_2_F, si_L_y_R_2_F / env.L_y_R_2_F * 1E3 * 1E2);
	printf("    L-R correlation: %.3f%%\n", corr_L_y * 1E2);

	//----------------------------------------------------------------------------------------------------

	printf("\n");
	printf("* eigenspectrum:\n");

	TMatrixDSymEigen eig_decomp(env.opt_cov);
	TVectorD eig_values(eig_decomp.GetEigenValues());
	TMatrixD eig_vectors(eig_decomp.GetEigenVectors());
	//TMatrixDSym S(16);
	for (unsigned int i = 0; i < 16; i++)
	{
		printf("    eigenvector %i --> eigenvalue %f\n", i, sqrt(eig_values(i)));

		/*
		printf("    ");
		for (unsigned int j = 0; j < 16; j++)
			printf("%+.2E, ", eig_vectors(j, i));
		printf("\n");
		*/
		printf("        L_x_L_2_F: %.4f, L_x_R_2_F: %.4f, L_y_L_2_F: %.4f, L_y_R_2_F: %.4f\n",
			eig_vectors(5, i), eig_vectors(13, i), eig_vectors(7, i), eig_vectors(15, i)
		);

		//S(i, i) = (eig_values(i) >= 0.) ? sqrt(eig_values(i)) : 0.;
	}
	
	//----------------------------------------------------------------------------------------------------

	StudyEigenModes(0, 0, env);

	StudyEigenModes(1, 1, env);

	StudyEigenModes(2, 2, env);

	StudyEigenModes(0, 2, env);

	StudyEigenModes(0, 15, env);

	return 0;
}
