void Analysis::BuildCuts()
{
	N_cuts = 9;

	// cut structure:
	//	| a*qa + b*qb + c| < n_si * si

	// a: th_x_R, b: th_x_L
	cqaN[1] = "#theta_{x}^{R}"; cqbN[1] = "#theta_{x}^{L}";
	cca[1] = -cut1_a;
	ccb[1] = 1.;
	ccc[1] = cut1_c;
	csi[1] = cut1_si;
	cuts.push_back(1);
	
	// a: th_y_R, b: th_y_L
	cqaN[2] = "#theta_{y}^{R}"; cqbN[2] = "#theta_{y}^{L}";
	cca[2] = -cut2_a;
	ccb[2] = 1.;
	ccc[2] = cut2_c;
	csi[2] = cut2_si;
	cuts.push_back(2);

	// a: th_x_R, b: vtx_x_R
	cqaN[3] = "#theta_{x}^{R}"; cqbN[3] = "vtx_{x}^{R}";
	cca[3] = -cut3_a;
	ccb[3] = 1.;
	ccc[3] = cut3_c;
	csi[3] = cut3_si;
	//cuts.push_back(3);
	
	// a: th_x_L, b: vtx_x_L
	cqaN[4] = "#theta_{x}^{L}"; cqbN[4] = "vtx_{x}^{L}";
	cca[4] = -cut4_a;
	ccb[4] = 1.;
	ccc[4] = cut4_c;
	csi[4] = cut4_si;
	//cuts.push_back(4);
	
	// a: y_R_1_F, b: y_R_2_F - y_R_1_F
	cqaN[5] = "y^{R,1,F}"; cqbN[5] = "y^{R,2,F} - y^{R,1,F}";
	cca[5] = -cut5_a;
	ccb[5] = 1.;
	ccc[5] = cut5_c;
	csi[5] = cut5_si;
	cuts.push_back(5);
	
	// a: y_L_1_F, b: y_L_2_F - y_L_1_F
	cqaN[6] = "y^{L,1,F}"; cqbN[6] = "y^{L,2,F} - y^{L,1,F}";
	cca[6] = -cut6_a;
	ccb[6] = 1.;
	ccc[6] = cut6_c;
	csi[6] = cut6_si;
	cuts.push_back(6);
	
	// a: th_x, b: vtx_x_R - vtx_x_L
	cqaN[7] = "#theta_{x}"; cqbN[7] = "vtx_{x}^{R} - vtx_{x}^{L}";
	cca[7] = -cut7_a;
	ccb[7] = 1.;
	ccc[7] = cut7_c;
	csi[7] = cut7_si;
	cuts.push_back(7);
	
	// a: th_y, b: vtx_y_R - vtx_y_L
	cqaN[8] = "#theta_{y}"; cqbN[8] = "vtx_{y}^{R} - vtx_{y}^{L}";
	cca[8] = -cut8_a;
	ccb[8] = 1.;
	ccc[8] = cut8_c;
	csi[8] = cut8_si;

	// also collinearity in th_x, but with input available in the 2-RP analysis
	// a: th_x_R_2_F, b: th_x_L_2_F
	cqaN[9] = "#theta_{x}^{R2F}"; cqbN[1] = "#theta_{x}^{L2F}";
	cca[9] = -cut1_a;
	ccb[9] = 1.;
	ccc[9] = cut1_c;
	csi[9] = 14E-6;
}

//----------------------------------------------------------------------------------------------------

bool Analysis::EvaluateCuts(const HitData & h, const Kinematics &k, CutData &cd) const
{
	cd.cqa[1] = k.th_x_R;	cd.cqb[1] = k.th_x_L;
	cd.cqa[2] = k.th_y_R;	cd.cqb[2] = k.th_y_L;
	cd.cqa[3] = k.th_x_R;	cd.cqb[3] = k.vtx_x_R;
	cd.cqa[4] = k.th_x_L;	cd.cqb[4] = k.vtx_x_L;
	cd.cqa[5] = h.R_1_F.y;	cd.cqb[5] = h.R_2_F.y - h.R_1_F.y;
	cd.cqa[6] = h.L_1_F.y;	cd.cqb[6] = h.L_2_F.y - h.L_1_F.y;
	cd.cqa[7] = k.th_x;		cd.cqb[7] = k.vtx_x_R - k.vtx_x_L;
	cd.cqa[8] = k.th_y;		cd.cqb[8] = k.vtx_y_R - k.vtx_y_L;

	cd.cqa[9] = k.th_x_R_2_F;	cd.cqb[9] = k.th_x_L_2_F;

	for (unsigned int ci = 1; ci <= N_cuts; ++ci)
	{
		cd.cv[ci] = cca[ci]*cd.cqa[ci] + ccb[ci]*cd.cqb[ci] + ccc[ci];
		cd.ct[ci] = (fabs(cd.cv[ci]) <= n_si * csi[ci]);
		//printf("cut %u: |%+E| < %E * %E <==> %i\n", ci, cd.cv[ci], n_si, csi[ci], cd.ct[ci]);
	}

	// and between all cuts
	bool select = true;	
	for (unsigned int ci = 0; ci < cuts.size(); ci++)
	{
		select &= cd.ct[cuts[ci]];
	}

	return select;
}
