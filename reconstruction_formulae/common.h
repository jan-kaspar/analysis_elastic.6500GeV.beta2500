//----------------------------------------------------------------------------------------------------

typedef Kinematics (*Func)(const HitData &, const Environment &);

//----------------------------------------------------------------------------------------------------

enum RecoQuantity
{
	rqTh_x = 0, rqTh_x_L, rqTh_x_R, rqTh_x_RLdiff, rqTh_x_LFdiff, rqTh_x_RFdiff, rqTh_x_LtoRratio,
	rqVtx_x = 100, rqVtx_x_L, rqVtx_x_R, rqVtx_x_RLdiff,
	rqTh_y = 200, rqTh_y_L, rqTh_y_R, rqTh_y_RLdiff, rqTh_y_LtoRratio
};

//----------------------------------------------------------------------------------------------------

double ExtractEffect(RecoQuantity q, const Kinematics &k_tr, const Kinematics &k_sm, const Kinematics &k_re)
{
	double q_tr=0., q_re=0.;
	if (q == rqTh_x_R) { q_tr = k_tr.th_x_R; q_re = k_re.th_x_R; }
	if (q == rqTh_x_L) { q_tr = k_tr.th_x_L; q_re = k_re.th_x_L; }
	if (q == rqTh_x) { q_tr = k_tr.th_x; q_re = k_re.th_x; }
	if (q == rqTh_x_RLdiff) { q_tr = k_tr.th_x_R - k_tr.th_x_L; q_re = k_re.th_x_R - k_re.th_x_L; }
	if (q == rqTh_x_LFdiff) { q_tr = k_tr.th_x_L - k_tr.th_x; q_re = k_re.th_x_L - k_re.th_x; }
	if (q == rqTh_x_RFdiff) { q_tr = k_tr.th_x_R - k_tr.th_x; q_re = k_re.th_x_R - k_re.th_x; }
	if (q == rqTh_x_LtoRratio) { q_tr = k_tr.th_x_L / k_tr.th_x_R; q_re = k_re.th_x_L / k_re.th_x_R; }

	if (q == rqVtx_x_R) { q_tr = k_sm.vtx_x; q_re = k_re.vtx_x_R; }
	if (q == rqVtx_x_L) { q_tr = k_sm.vtx_x; q_re = k_re.vtx_x_L; }
	if (q == rqVtx_x) { q_tr = k_sm.vtx_x; q_re = k_re.vtx_x; }
	if (q == rqVtx_x_RLdiff) { q_tr = 0.; q_re = k_re.vtx_x_R - k_re.vtx_x_L; }

	if (q == rqTh_y_R) { q_tr = k_tr.th_y_R; q_re = k_re.th_y_R; }
	if (q == rqTh_y_L) { q_tr = k_tr.th_y_L; q_re = k_re.th_y_L; }
	if (q == rqTh_y) { q_tr = k_tr.th_y; q_re = k_re.th_y; }
	if (q == rqTh_y_RLdiff) { q_tr = k_tr.th_y_R - k_tr.th_y_L; q_re = k_re.th_y_R - k_re.th_y_L; }
	if (q == rqTh_y_LtoRratio) { q_tr = k_tr.th_y_L / k_tr.th_y_R; q_re = k_re.th_y_L / k_re.th_y_R; }

	return q_re - q_tr;
}

//----------------------------------------------------------------------------------------------------

enum SimuBits { sbPitch=1, sbBeamDivergence=2, sbMisalignment=4, sbVertex=8, sbAngle=16, sbOptics=32 };

//----------------------------------------------------------------------------------------------------

Environment env_nom;

//----------------------------------------------------------------------------------------------------

void AddString(string &s, const string &add)
{
	if (s.empty())
		s = add;
	else
		s += "," + add;
}

//----------------------------------------------------------------------------------------------------

string SimuCodeToString(unsigned int code)
{
	string s;

	if (code & sbPitch) AddString(s, "pitch");
	if (code & sbBeamDivergence) AddString(s, "beamDiv");
	if (code & sbMisalignment) AddString(s, "misalig");
	if (code & sbVertex) AddString(s, "vertex");
	if (code & sbAngle) AddString(s, "angle");
	if (code & sbOptics) AddString(s, "optics");

	return s;
}

//----------------------------------------------------------------------------------------------------

Stat st_opt(16);

void GenerateOpticsPerturbation(Environment &env)
{
	TVectorD de(16);
	env.ApplyRandomOpticsPerturbations(de);
	st_opt.Fill(de);
}

//----------------------------------------------------------------------------------------------------

void SimulateEvent(const Kinematics &k_cv, unsigned int b, Func f_reco, Kinematics &k_tr, Kinematics &k_sm, Kinematics &k_re)
{
	// true
	k_tr = k_cv;

	// generate scattering angles
	if (b & sbAngle)
	{
		double B = 20.;
		double si_th_x = sqrt(1. / (2. * B * env_nom.p * env_nom.p)), si_th_y = si_th_x;
		k_tr.th_x = k_tr.th_x_L = k_tr.th_x_R = gRandom->Gaus() * si_th_x;
		k_tr.th_y = k_tr.th_y_L = k_tr.th_y_R = gRandom->Gaus() * si_th_y;
	}

	// simulation environment
	Environment env_sim = env_nom;

	// beam divergence + vertex
	k_sm = k_tr;
	if (b & sbBeamDivergence)
	{
		k_sm.th_x_L += gRandom->Gaus() * env_sim.si_th_x_L;
		k_sm.th_y_L += gRandom->Gaus() * env_sim.si_th_y_L;
		k_sm.th_x_R += gRandom->Gaus() * env_sim.si_th_x_R;
		k_sm.th_y_R += gRandom->Gaus() * env_sim.si_th_y_R;
	}

	if (b & sbVertex)
	{
		k_sm.vtx_x += gRandom->Gaus() * env_sim.si_vtx_x;
		k_sm.vtx_y += gRandom->Gaus() * env_sim.si_vtx_y;
	}

	// actual optics
	if (b & sbOptics)
	{
		GenerateOpticsPerturbation(env_sim);
	}

	// proton transport
	HitData h = ProtonTransport(k_sm, env_sim);
	
	// misalignment
	if (b & sbMisalignment) 
	{
		double de_y_R = gRandom->Gaus() * env_sim.si_de_y_R;
		double de_y_D = gRandom->Gaus() * env_sim.si_de_y_D;

		h.L_2_F.x += gRandom->Gaus() * env_sim.si_de_x; h.L_2_F.y += -de_y_R + de_y_D;
		h.L_1_F.x += gRandom->Gaus() * env_sim.si_de_x; h.L_1_F.y += -de_y_R + de_y_D;
		h.R_1_F.x += gRandom->Gaus() * env_sim.si_de_x; h.R_1_F.y += +de_y_R;
		h.R_2_F.x += gRandom->Gaus() * env_sim.si_de_x; h.R_2_F.y += +de_y_R;
	}

	// pitch error
	if (b & sbPitch)
	{
		h.L_2_F.x += gRandom->Gaus() * env_sim.si_de_P_L; h.L_2_F.y += gRandom->Gaus() * env_sim.si_de_P_L;
		h.L_1_F.x += gRandom->Gaus() * env_sim.si_de_P_L; h.L_1_F.y += gRandom->Gaus() * env_sim.si_de_P_L;
		h.R_1_F.x += gRandom->Gaus() * env_sim.si_de_P_R; h.R_1_F.y += gRandom->Gaus() * env_sim.si_de_P_R;
		h.R_2_F.x += gRandom->Gaus() * env_sim.si_de_P_R; h.R_2_F.y += gRandom->Gaus() * env_sim.si_de_P_R;
	}

	// reconstruction
	Environment env_rec = env_nom;
	k_re = f_reco(h, env_rec);
}
