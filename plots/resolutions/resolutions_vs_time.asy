import root;
import pad_layout;
include "../run_info.asy";

string datasets[], dataset_fills[];
datasets.push("DS-fill5313"); dataset_fills.push("5313");
datasets.push("DS-fill5314"); dataset_fills.push("5314");
datasets.push("DS-fill5317"); dataset_fills.push("5317");
datasets.push("DS-fill5321"); dataset_fills.push("5321");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };
pen dgn_pens[] = { blue, red };

string topDir = "../../";

xSizeDef = 10cm;
ySizeDef = 6cm;

xTicksDef = LeftTicks(1., 0.5);

TGraph_errorBar = None;

//TGraph_reducePoints = 10;

//----------------------------------------------------------------------------------------------------

real beta_st = 2500;	// m

//----------------------------------------------------------------------------------------------------

void SetPadWidth()
{
	real factorHoursToSize = 10cm / 3;

	real timespan = currentpicture.userMax2().x - currentpicture.userMin2().x;
	currentpad.xSize = timespan * factorHoursToSize;
}

//----------------------------------------------------------------------------------------------------

void DrawBeamDivergence(RootObject g_vtx_rms, pen p, marker m, string label)
{
	int N = g_vtx_rms.iExec("GetN");
	for (int i = 0; i < N; ++i)
	{
		real xa[] = { 0 };
		real ya[] = { 0 };
		g_vtx_rms.vExec("GetPoint", i, xa, ya);
		real time = xa[0];
		real vtx_rms = ya[0] / 1e3;	// in m
		real vtx_rms_unc = g_vtx_rms.rExec("GetErrorY", i) / 1e3;

		real bd = sqrt(2) * vtx_rms / beta_st;
		real bd_unc = sqrt(2) * vtx_rms_unc / beta_st;

		if (bd != 0)
		{
			draw(swToHours*scale(1, 1e6)*(time, bd), p, m);
			draw(swToHours*scale(1, 1e6)*((time, bd - bd_unc)--(time, bd + bd_unc)), p);
		}
	}
}

//----------------------------------------------------------------------------------------------------

void DrawMeanSensorResolution(RootObject g_vtx_rms, RootObject g_diffRL_th_x, pen p, marker m, string label)
{
	int N = g_vtx_rms.iExec("GetN");
	for (int i = 0; i < N; ++i)
	{
		real xa[] = { 0 };
		real ya[] = { 0 };
		g_vtx_rms.vExec("GetPoint", i, xa, ya);
		real time = xa[0];
		real vtx_rms = ya[0] / 1e3;	// in m
		real vtx_rms_unc = g_vtx_rms.rExec("GetErrorY", i) / 1e3;
		
		g_diffRL_th_x.vExec("GetPoint", i, xa, ya);
		real diff = ya[0];
		real diff_unc = g_diffRL_th_x.rExec("GetErrorY", i);

		real bd = sqrt(2) * vtx_rms / beta_st;	// rad
		real bd_unc = sqrt(2) * vtx_rms_unc / beta_st;

		real rr = sqrt(diff*diff/2 - bd*bd); // rad
		real rr_unc = (rr > 0) ? sqrt(diff*diff*diff_unc*diff_unc + 4.*bd*bd*bd_unc*bd_unc) / 2 / rr: 0.;

		//real lambda_mean = 0.539;
		real lambda_mean = 0.119;
		rr /= lambda_mean;
		rr_unc /= lambda_mean;

		if (bd != 0)
		{
			draw(swToHours*scale(1, 1e6)*(time, rr), p, m);
			draw(swToHours*scale(1, 1e6)*((time, rr-rr_unc)--(time, rr+rr_unc)), p);
		}
	}
}

//----------------------------------------------------------------------------------------------------

void DrawFinalThXResolution(RootObject g_vtx_rms, RootObject g_diffRL_th_x, real msr, pen p, marker m, string label)
{
	int N = g_vtx_rms.iExec("GetN");
	for (int i = 0; i < N; ++i)
	{
		real xa[] = { 0 };
		real ya[] = { 0 };
		g_vtx_rms.vExec("GetPoint", i, xa, ya);
		real time = xa[0];
		real vtx_rms = ya[0] / 1e3;	// in m
		real vtx_rms_unc = g_vtx_rms.rExec("GetErrorY", i) / 1e3;
		
		g_diffRL_th_x.vExec("GetPoint", i, xa, ya);
		real diff = ya[0];
		real diff_unc = g_diffRL_th_x.rExec("GetErrorY", i);

		real bd = sqrt(2) * vtx_rms / beta_st;
		real bd_unc = sqrt(2) * vtx_rms_unc / beta_st;

		real si_sr = msr / 111.8;

		real res = sqrt(bd*bd/2 + si_sr*si_sr);
		real res_unc = bd * bd_unc / 2 / res;

		if (bd != 0)
		{
			draw(swToHours*scale(1, 1e6)*(time, res), p, m);
			draw(swToHours*scale(1, 1e6)*((time, res-res_unc)--(time, res+res_unc)), p);
		}
	}
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
NewRow();

for (int dsi : datasets.keys)
{
	NewPad("time $\ung{h}$", "std.~dev.~of $\De^{R-L}\th_x^*\ung{\mu rad}$");
	//currentpad.yTicks = RightTicks(0.1, 0.05);
	real y_min = 10, y_max = 15;
	
	DrawRunBands(dataset_fills[dsi], y_min, y_max);

	for (int dgni : diagonals.keys)
	{
		draw(swToHours*scale(1, 1e6),
			RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "time dependences/gRMS_diffLR_th_x_vs_time"), "p,eb,d0",
			dgn_pens[dgni], mCi+2pt+dgn_pens[dgni], dgn_labels[dgni]);

	}

	ylimits(y_min, y_max, Crop);
	SetPadWidth();
}

//----------------------------------------------------------------------------------------------------
/*
NewRow();

for (int dsi : datasets.keys)
{
	NewPad("time $\ung{h}$", "std.~dev.~of $x^*\ung{\mu m}$");
	currentpad.yTicks = RightTicks(50., 10.);
	real y_min = 450, y_max = 750;

	DrawRunBands(dataset_fills[dsi], y_min, y_max);

	for (int dgni : diagonals.keys)
	{
		draw(swToHours*scale(1, 1e3),
			RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "time dependences/gRMS_vtx_x_vs_time"), "p,eb,d0",
			dgn_pens[dgni], mCi+2pt+dgn_pens[dgni], dgn_labels[dgni]);
	}

	ylimits(y_min, y_max, Crop);
	SetPadWidth();
}

//----------------------------------------------------------------------------------------------------
NewRow();

for (int dsi : datasets.keys)
{
	NewPad("time $\ung{h}$", "\vbox{\hbox{beam divergence in x $\ung{\mu m}$}\hbox{$\displaystyle\sqrt 2 \,\si[x^*]\over \be^*$}}");
	currentpad.yTicks = RightTicks(0.01, 0.005);
	real y_min = 0.27, y_max = 0.4;

	DrawRunBands(dataset_fills[dsi], y_min, y_max);

	for (int dgni : diagonals.keys)
	{
		RootObject obj_vtx_x = RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "time dependences/gRMS_vtx_x_vs_time");
		RootObject obj_diffRL_vtx_x = RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "time dependences/gRMS_diffLR_th_x_vs_time");

		DrawBeamDivergence(obj_vtx_x, dgn_pens[dgni], mCi+2pt+dgn_pens[dgni], dgn_labels[dgni]);
	}

	ylimits(y_min, y_max, Crop);
	SetPadWidth();
}

//----------------------------------------------------------------------------------------------------
NewRow();

for (int dsi : datasets.keys)
{
	NewPad("time $\ung{h}$", "\vbox{\hbox{mean RP resolution $\ung{\mu m}$}\hbox{$\displaystyle\sqrt{\si[\De^{R-L}\th_x^*]^2/2 - \si_{\rm beam\ div.}^2} \over \hbox{mean } {\sqrt{v^N v^N + v^F v^F}\over v^N L^F - v^F L^N}$}}");
	currentpad.yTicks = RightTicks(0.2, 0.1);
	real y_min = 10, y_max = 13;

	DrawRunBands(dataset_fills[dsi], y_min, y_max);

	for (int dgni : diagonals.keys)
	{
		RootObject obj_vtx_x = RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "time dependences/gRMS_vtx_x_vs_time");
		RootObject obj_diffRL_vtx_x = RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "time dependences/gRMS_diffLR_th_x_vs_time");

		DrawMeanSensorResolution(obj_vtx_x, obj_diffRL_vtx_x, dgn_pens[dgni], mCi+2pt+dgn_pens[dgni], dgn_labels[dgni]);
	}

	xaxis(YEquals(11.8, false), blue);
	xaxis(YEquals(11.0, false), red);

	ylimits(y_min, y_max, Crop);
	SetPadWidth();
}
*/

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

NewRow();

for (int dsi : datasets.keys)
{
	NewPad("time $\ung{h}$", "std.~dev.~of $\De^{R-L}\th_y^*\ung{\mu rad}$");
	currentpad.yTicks = RightTicks(0.01, 0.005);
	real y_min = 0.33, y_max = 0.43;

	DrawRunBands(dataset_fills[dsi], y_min, y_max);

	for (int dgni : diagonals.keys)
	{
		draw(swToHours*scale(1, 1e6),
			RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "time dependences/gRMS_diffLR_th_y_vs_time"), "p,eb,d0",
			dgn_pens[dgni], mCi+2pt+dgn_pens[dgni], dgn_labels[dgni]);
	}
	
	ylimits(y_min, y_max, Crop);
	SetPadWidth();
}

frame f_leg = BuildLegend();

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

NewRow();

NewPad(false);
attach(f_leg);

GShipout(vSkip=0mm);
