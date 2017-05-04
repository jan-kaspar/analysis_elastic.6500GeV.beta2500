import root;
import pad_layout;
import common_code;

AddAllModes();

string topDir = "../../";

string datasets[] = { "DS-fill5313" };

string diagonal = "combined";

string objects[] = {
//	"g_eff_45b",
//	"g_eff_45t",
	"g_eff_comb1",
	"g_eff_comb2",
};

string object_labels[] = {
//	"45b-56t",
//	"45t-56b",
	"dgn. combination",
	"2nd dgn. combination",
};

real z_t_maxs[], z_t_Steps[], z_t_steps[], z_e_maxs[], z_e_Steps[], z_e_steps[];
z_t_maxs.push(0.004); z_t_Steps.push(0.002); z_t_steps.push(0.001); z_e_maxs.push(0.02); z_e_Steps.push(0.005); z_e_steps.push(0.001);
z_t_maxs.push(0.2); z_t_Steps.push(0.05); z_t_steps.push(0.01); z_e_maxs.push(0.02); z_e_Steps.push(0.005); z_e_steps.push(0.001);
z_t_maxs.push(1.0); z_t_Steps.push(0.2); z_t_steps.push(0.1); z_e_maxs.push(0.04); z_e_Steps.push(0.01); z_e_steps.push(0.005);

string binning = "ob-3-5-0.05";

//----------------------------------------------------------------------------------------------------

void PlotAllModes(string f)
{
	int ci = 0;

	for (int mi : modes.keys)
	{
		for (int oi : objects.keys)
		{
			string pth = "contributions/" + modes[mi].num_int_tag + "/" + objects[oi];
			RootObject obj = RootGetObject(f, pth, error=false);
			if (obj.valid)
			{
				draw(scale(1, 100), obj, "l,d0", StdPen(ci+1), modes[mi].label + " ("+ replace(object_labels[oi], "_", "\_") + ")");
				++ci;
			}
		}
	}

	draw(scale(1, +100), RootGetObject(f, "matrices/all-but-norm/"+diagonal+"/g_envelope"), "l", black+1pt, "$\pm 1\un{\si}$ {\it envelope of uncertainties (without normalisation)}");
	draw(scale(1, -100), RootGetObject(f, "matrices/all-but-norm/"+diagonal+"/g_envelope"), "l", black+1pt);
}


//----------------------------------------------------------------------------------------------------

frame f_legend;

int gy = 0;

for (int dsi : datasets.keys)
{
	++gy;

	string f = topDir + "systematics/"+datasets[dsi]+"/matrix_numerical_integration.root";
	
	NewPad(false, -1, gy);
	label("{\SetFontSizesXX "+replace(datasets[dsi], "_", "\_")+"}");

	int gx = 0;

	for (int zi : z_t_maxs.keys)
	{
		NewPad("$|t|\ung{GeV^2}$", "systematic effect$\ung{\%}$", gx, gy);

		PlotAllModes(f);

		real t_Step = z_t_Steps[zi];
		real t_step = z_t_steps[zi];
		real e_Step = z_e_Steps[zi] * 100;
		real e_step = z_e_steps[zi] * 100;

		real t_min = 0;
		real t_max = z_t_maxs[zi];
		real e_min = -z_e_maxs[zi] * 100;
		real e_max = z_e_maxs[zi] * 100;

		currentpad.xTicks = LeftTicks(t_Step, t_step);
		currentpad.yTicks = RightTicks(e_Step, e_step);
		
		limits((0, -e_max), (t_max, e_max), Crop);
		
		xaxis(YEquals(0, false), dashed);
		yaxis(XEquals(8e-4, false), dashed);

		f_legend = BuildLegend();
		currentpicture.legend.delete();

		++gx;
	}


	//--------------------

	//gx = 0;
	//++gy;
	
	if (false)
	{
		NewPad("$|t|\ung{GeV^2}$", "$|t|\ung{GeV^2}$", axesAbove=true, gx, gy);
		currentpad.xTicks = LeftTicks(0.05, 0.01);
		currentpad.yTicks = RightTicks(0.05, 0.01);
		TH2_z_min = -1;
		TH2_z_max = +1;
		draw(RootGetObject(f, "matrices/all-but-norm/"+diagonal+"/"+binning+"/h_corr_mat"), "p,bar");
		limits((0, 0), (0.20, 0.20), Crop);
		//AttachLegend("analysis uncertainties -- correlation matrix", SE, NE);
	}

	//--------------------

	++gx;
	NewPad(false, gx, gy);
	//add(shift(0, 345) * f_legend);
	add(shift(0, +100) * f_legend);
}

GShipout();
