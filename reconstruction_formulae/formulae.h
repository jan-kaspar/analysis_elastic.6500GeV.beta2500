//----------------------------------------------------------------------------------------------------
// theta_x, one pot
//----------------------------------------------------------------------------------------------------

Kinematics theta_x_one_pot_hit_L2F(const HitData &h, const Environment &env)
{
	Kinematics k;
	k.th_x_L = k.th_x_L_2_F = - h.L_2_F.x / env.L_x_L_2_F;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_x_one_pot_hit_L1F(const HitData &h, const Environment &env)
{
	Kinematics k;
	k.th_x_L = k.th_x_L_1_F = - h.L_1_F.x / env.L_x_L_1_F;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_x_one_pot_hit_R1F(const HitData &h, const Environment &env)
{
	Kinematics k;
	k.th_x_R = k.th_x_R_1_F = + h.R_1_F.x / env.L_x_R_1_F;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_x_one_pot_hit_R2F(const HitData &h, const Environment &env)
{
	Kinematics k;
	k.th_x_R = k.th_x_R_2_F = + h.R_2_F.x / env.L_x_R_2_F;
	return k;
}

//----------------------------------------------------------------------------------------------------
// theta_x, one arm
//----------------------------------------------------------------------------------------------------

Kinematics theta_x_one_arm_hit(const HitData &h, const Environment &env)
{
	Kinematics k;
	k.th_x_R = + (h.R_1_F.x / env.L_x_R_1_F + h.R_2_F.x / env.L_x_R_2_F) / 2.;
	k.th_x_L = - (h.L_1_F.x / env.L_x_L_1_F + h.L_2_F.x / env.L_x_L_2_F) / 2.;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_x_one_arm_angle(const HitData &h, const Environment &env)
{
	Kinematics k;
	k.th_x_R = + (h.R_2_F.x - h.R_1_F.x) / (env.L_x_R_2_F - env.L_x_R_1_F);
	k.th_x_L = - (h.L_2_F.x - h.L_1_F.x) / (env.L_x_L_2_F - env.L_x_L_1_F);
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_x_one_arm_regr(const HitData &h, const Environment &env)
{
	Kinematics k;

	double D_x_L = - env.L_x_L_1_F * env.v_x_L_2_F + env.L_x_L_2_F * env.v_x_L_1_F;
	k.th_x_L = (env.v_x_L_2_F * h.L_1_F.x - env.v_x_L_1_F * h.L_2_F.x) / D_x_L;
	//k.vtx_x_L = (env.L_x_L_2_F * h.L_1_F.x - env.L_x_L_1_F * h.L_2_F.x) / D_x_L;

	double D_x_R = + env.L_x_R_1_F * env.v_x_R_2_F - env.L_x_R_2_F * env.v_x_R_1_F;
	k.th_x_R = (env.v_x_R_2_F * h.R_1_F.x - env.v_x_R_1_F * h.R_2_F.x) / D_x_R;
	//k.vtx_x_R = (-env.L_x_R_2_F * h.R_1_F.x + env.L_x_R_1_F * h.R_2_F.x) / D_x_R;

	return k;
}

//----------------------------------------------------------------------------------------------------
// theta_x, two arm
//----------------------------------------------------------------------------------------------------

Kinematics theta_x_two_arm_hit_2F_LRavg(const HitData &h, const Environment &env)
{
	Kinematics k;

	k.th_x_R = + h.R_2_F.x / env.L_x_R_2_F;
	k.th_x_L = - h.L_2_F.x / env.L_x_L_2_F;

	k.th_x = (k.th_x_R + k.th_x_L) / 2.;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_x_two_arm_hit_LRavg(const HitData &h, const Environment &env)
{
	Kinematics k = theta_x_one_arm_hit(h, env);
	k.th_x = (k.th_x_R + k.th_x_L) / 2.;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_x_two_arm_angle_LRavg(const HitData &h, const Environment &env)
{
	Kinematics k = theta_x_one_arm_angle(h, env);
	k.th_x = (k.th_x_R + k.th_x_L) / 2.;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_x_one_arm_regr_LRavg(const HitData &h, const Environment &env)
{
	Kinematics k = theta_x_one_arm_regr(h, env);
	k.th_x = (k.th_x_R + k.th_x_L) / 2.;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_x_two_arm_full_regr(const HitData &h, const Environment &env)
{
	Kinematics k;

	double SLL = + env.L_x_L_2_F*env.L_x_L_2_F + env.L_x_L_1_F*env.L_x_L_1_F + env.L_x_R_1_F*env.L_x_R_1_F + env.L_x_R_2_F*env.L_x_R_2_F;
	double SLv = - env.L_x_L_2_F*env.v_x_L_2_F - env.L_x_L_1_F*env.v_x_L_1_F + env.L_x_R_1_F*env.v_x_R_1_F + env.L_x_R_2_F*env.v_x_R_2_F;
	double Svv = + env.v_x_L_2_F*env.v_x_L_2_F + env.v_x_L_1_F*env.v_x_L_1_F + env.v_x_R_1_F*env.v_x_R_1_F + env.v_x_R_2_F*env.v_x_R_2_F;
	double D = (SLL * Svv - SLv * SLv);
	
	double SLx = - env.L_x_L_2_F*h.L_2_F.x - env.L_x_L_1_F*h.L_1_F.x + env.L_x_R_1_F*h.R_1_F.x + env.L_x_R_2_F*h.R_2_F.x;
	double Svx = + env.v_x_L_2_F*h.L_2_F.x + env.v_x_L_1_F*h.L_1_F.x + env.v_x_R_1_F*h.R_1_F.x + env.v_x_R_2_F*h.R_2_F.x;
		
	k.th_x = (Svv * SLx - SLv * Svx) / D;
	k.vtx_x = (-SLv * SLx + SLL * Svx) / D;

	return k;
}


//----------------------------------------------------------------------------------------------------
// vtx_x, one arm
//----------------------------------------------------------------------------------------------------

Kinematics vtx_x_one_arm_regr(const HitData &h, const Environment &env)
{
	Kinematics k;

	double D_x_L = - env.L_x_L_1_F * env.v_x_L_2_F + env.L_x_L_2_F * env.v_x_L_1_F;
	k.th_x_L = (env.v_x_L_2_F * h.L_1_F.x - env.v_x_L_1_F * h.L_2_F.x) / D_x_L;
	k.vtx_x_L = (env.L_x_L_2_F * h.L_1_F.x - env.L_x_L_1_F * h.L_2_F.x) / D_x_L;

	double D_x_R = + env.L_x_R_1_F * env.v_x_R_2_F - env.L_x_R_2_F * env.v_x_R_1_F;
	k.th_x_R = (env.v_x_R_2_F * h.R_1_F.x - env.v_x_R_1_F * h.R_2_F.x) / D_x_R;
	k.vtx_x_R = (-env.L_x_R_2_F * h.R_1_F.x + env.L_x_R_1_F * h.R_2_F.x) / D_x_R;

	return k;
}
	
//----------------------------------------------------------------------------------------------------
// vtx_x, two arm
//----------------------------------------------------------------------------------------------------

Kinematics vtx_x_one_arm_regr_LRavg(const HitData &h, const Environment &env)
{
	Kinematics k = vtx_x_one_arm_regr(h, env);

	k.vtx_x = (k.vtx_x_L + k.vtx_x_R) / 2.;

	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics vtx_x_two_arm_full_regr(const HitData &h, const Environment &env)
{
	Kinematics k;

	double SLL = + env.L_x_L_2_F*env.L_x_L_2_F + env.L_x_L_1_F*env.L_x_L_1_F + env.L_x_R_1_F*env.L_x_R_1_F + env.L_x_R_2_F*env.L_x_R_2_F;
	double SLv = - env.L_x_L_2_F*env.v_x_L_2_F - env.L_x_L_1_F*env.v_x_L_1_F + env.L_x_R_1_F*env.v_x_R_1_F + env.L_x_R_2_F*env.v_x_R_2_F;
	double Svv = + env.v_x_L_2_F*env.v_x_L_2_F + env.v_x_L_1_F*env.v_x_L_1_F + env.v_x_R_1_F*env.v_x_R_1_F + env.v_x_R_2_F*env.v_x_R_2_F;
	double D = (SLL * Svv - SLv * SLv);
	
	double SLx = - env.L_x_L_2_F*h.L_2_F.x - env.L_x_L_1_F*h.L_1_F.x + env.L_x_R_1_F*h.R_1_F.x + env.L_x_R_2_F*h.R_2_F.x;
	double Svx = + env.v_x_L_2_F*h.L_2_F.x + env.v_x_L_1_F*h.L_1_F.x + env.v_x_R_1_F*h.R_1_F.x + env.v_x_R_2_F*h.R_2_F.x;
		
	k.th_x = (Svv * SLx - SLv * Svx) / D;
	k.vtx_x = (-SLv * SLx + SLL * Svx) / D;

	return k;
}

//----------------------------------------------------------------------------------------------------
// theta_y, one pot
//----------------------------------------------------------------------------------------------------

Kinematics theta_y_one_pot_hit_L2F(const HitData &h, const Environment &env)
{
	Kinematics k;
	k.th_y_L = k.th_y_L_2_F = - h.L_2_F.y / env.L_y_L_2_F;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_y_one_pot_hit_L1F(const HitData &h, const Environment &env)
{
	Kinematics k;
	k.th_y_L = k.th_y_L_1_F = - h.L_1_F.y / env.L_y_L_1_F;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_y_one_pot_hit_R1F(const HitData &h, const Environment &env)
{
	Kinematics k;
	k.th_y_R = k.th_y_R_1_F = + h.R_1_F.y / env.L_y_R_1_F;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_y_one_pot_hit_R2F(const HitData &h, const Environment &env)
{
	Kinematics k;
	k.th_y_R = k.th_y_R_2_F = + h.R_2_F.y / env.L_y_R_2_F;
	return k;
}

//----------------------------------------------------------------------------------------------------
// theta_y, one arm
//----------------------------------------------------------------------------------------------------

Kinematics theta_y_one_arm_hit(const HitData &h, const Environment &env)
{
	Kinematics k;
	k.th_y_R = + (h.R_1_F.y / env.L_y_R_1_F + h.R_2_F.y / env.L_y_R_2_F) / 2.;
	k.th_y_L = - (h.L_1_F.y / env.L_y_L_1_F + h.L_2_F.y / env.L_y_L_2_F) / 2.;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_y_one_arm_angle(const HitData &h, const Environment &env)
{
	Kinematics k;
	k.th_y_R = + (h.R_2_F.y - h.R_1_F.y) / (env.L_y_R_2_F - env.L_y_R_1_F);
	k.th_y_L = - (h.L_2_F.y - h.L_1_F.y) / (env.L_y_L_2_F - env.L_y_L_1_F);
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_y_one_arm_regr(const HitData &h, const Environment &env)
{
	Kinematics k;
	k.th_y_R = + (env.L_y_R_2_F*h.R_2_F.y + env.L_y_R_1_F*h.R_1_F.y) / (env.L_y_R_2_F*env.L_y_R_2_F + env.L_y_R_1_F*env.L_y_R_1_F);
	k.th_y_L = - (env.L_y_L_2_F*h.L_2_F.y + env.L_y_L_1_F*h.L_1_F.y) / (env.L_y_L_2_F*env.L_y_L_2_F + env.L_y_L_1_F*env.L_y_L_1_F);
	return k;
}

//----------------------------------------------------------------------------------------------------
// theta_y, two arm
//----------------------------------------------------------------------------------------------------

Kinematics theta_y_two_arm_hit_2F_LRavg(const HitData &h, const Environment &env)
{
	Kinematics k;

	k.th_y_R = + h.R_2_F.y / env.L_y_R_2_F;
	k.th_y_L = - h.L_2_F.y / env.L_y_L_2_F;

	k.th_y = (k.th_y_R + k.th_y_L) / 2.;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_y_two_arm_hit_LRavg(const HitData &h, const Environment &env)
{
	Kinematics k = theta_y_one_arm_hit(h, env);
	k.th_y = (k.th_y_R + k.th_y_L) / 2.;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_y_two_arm_angle_LRavg(const HitData &h, const Environment &env)
{
	Kinematics k = theta_y_one_arm_angle(h, env);
	k.th_y = (k.th_y_R + k.th_y_L) / 2.;
	return k;
}

//----------------------------------------------------------------------------------------------------

Kinematics theta_y_two_arm_simple_regr(const HitData &h, const Environment &env)
{
	Kinematics k;

	double SLL = env.L_y_L_2_F*env.L_y_L_2_F + env.L_y_L_1_F*env.L_y_L_1_F + env.L_y_R_1_F*env.L_y_R_1_F + env.L_y_R_2_F*env.L_y_R_2_F;
	double SLy =  - env.L_y_L_2_F*h.L_2_F.y - env.L_y_L_1_F*h.L_1_F.y + env.L_y_R_1_F*h.R_1_F.y + env.L_y_R_2_F*h.R_2_F.y;

	k.th_y = SLy / SLL;

	return k;
}
