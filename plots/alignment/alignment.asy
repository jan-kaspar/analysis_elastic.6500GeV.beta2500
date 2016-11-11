import root;
import pad_layout;
include "../run_info.asy";

string topDir = "../../";

string datasets[], fills[];
datasets.push("DS-fill5313"); fills.push("5313");
datasets.push("DS-fill5314"); fills.push("5314");
datasets.push("DS-fill5317"); fills.push("5317");
datasets.push("DS-fill5321"); fills.push("5321");

string units[], unit_labels[];
units.push("L_2_F"); unit_labels.push("L-220-fr");
units.push("L_1_F"); unit_labels.push("L-210-fr");
units.push("R_1_F"); unit_labels.push("R-210-rf");
units.push("R_2_F"); unit_labels.push("R-220-fr" );

xSizeDef = 10cm;
xTicksDef = LeftTicks(Step=1., step=0.5);
drawGridDef = true;

TGraph_errorBar = None;

bool centreToFit = true;

//----------------------------------------------------------------------------------------------------

void SetPadWidth()
{
	real timespan = currentpicture.userMax2().x - currentpicture.userMin2().x;
	currentpad.xSize = 10cm * timespan/10;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	string dataset = datasets[dsi];

	write(dataset);

	for (int ui : units.keys)
	{
		NewPad(false);
		label("{\SetFontSizesXX " + unit_labels[ui] + "}");
	}

	//----------------------------------------------------------------------------------------------------
	NewRow();

	for (int ui : units.keys)
	{
		NewPad("time $\ung{h}$", "tilt $\ung{mrad}$", axesAbove=false);
		currentpad.yTicks = RightTicks(2., 1.1);

		real y_cen = 0.;
		if (centreToFit)
		{
			RootObject fit = RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/a_fit");
			y_cen = fit.rExec("Eval", 0.);
		}
			
		real y_min = y_cen - 5, y_max = y_cen + 5;

		DrawRunBands(fills[dsi], y_min, y_max);
	
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/a_p"), "p,eb", magenta, mCi+1pt+magenta);
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/a_g"), "p,eb", heavygreen, mCi+1pt+heavygreen);
		
		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/a"), "p,eb", blue, mCi+1pt+blue);
		
		real unc = 2;
		draw(shift(0, +unc)*swToHours, RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/a_fit"), "l", red+dashed);
		draw(shift(0,    0)*swToHours, RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/a_fit"), "l", red+2pt);
		draw(shift(0, -unc)*swToHours, RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/a_fit"), "l", red+dashed);
	
		ylimits(y_min, y_max, Crop);
		//AttachLegend(unit_labels[ui], SE, SE);

		SetPadWidth();
	}
	
	//----------------------------------------------------------------------------------------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("time $\ung{h}$", "horizontal position $\ung{\mu m}$", axesAbove=false);
		currentpad.yTicks = RightTicks(10., 5.);

		real y_cen = 0;
		if (centreToFit)
		{
			RootObject fit = RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/b_fit");
			y_cen = fit.rExec("Eval", 0.);
		}

		real y_min = y_cen - 50, y_max = y_cen + 50;

		DrawRunBands(fills[dsi], y_min, y_max);
	
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/b_p"), "p,eb", magenta, mCi+1pt+magenta);
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/b_g"), "p,eb", heavygreen, mCi+1pt+heavygreen);
	
		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/b"), "p,eb", blue+1pt, mCi+1pt+blue);
		
		real unc = 25;
		draw(shift(0, -unc)*swToHours, RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/b_fit"), "l", red+dashed);
		draw(shift(0,    0)*swToHours, RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/b_fit"), "l", red+2pt);
		draw(shift(0, +unc)*swToHours, RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/b_fit"), "l", red+dashed);
	
		ylimits(y_min, y_max, Crop);
		//AttachLegend(unit_labels[ui], SE, SE);

		SetPadWidth();
	}
	
	//----------------------------------------------------------------------------------------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("time $\ung{h}$", "vertical position $\ung{\mu m}$", axesAbove=false);
		currentpad.yTicks = RightTicks(100., 50.);

		real y_cen = 0;
		if (centreToFit)
		{
			RootObject fit = RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/c_fit");
			y_cen = fit.rExec("Eval", 0.);
		}

		real y_min = y_cen - 200, y_max = y_cen + 200;

		DrawRunBands(fills[dsi], y_min, y_max);
	
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_min_diff"), "p,eb", cyan, mCi+1pt+cyan);
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_prob"), "p,eb", blue, mCi+1pt+blue);
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_mean_diff_sq"), "p,eb", magenta, mCi+1pt+magenta);
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_hist_chi_sq"), "p,eb", heavygreen, mCi+1pt+heavygreen);
			
		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c"), "p,l,eb", blue+1pt, mCi+1pt+blue);
		
		real unc = 100;
		draw(swToHours*shift(0, -unc), RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/c_fit"), "l", red+dashed);
		draw(swToHours*shift(0,    0), RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/c_fit"), "l", red+2pt);
		draw(swToHours*shift(0, +unc), RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/c_fit"), "l", red+dashed);
	
		ylimits(y_min, y_max, Crop);
		//AttachLegend(unit_labels[ui], SE, SE);

		SetPadWidth();
	}
	
	//----------------------------------------------------------------------------------------------------
	
	GShipout("alignment_" + dataset, hSkip=1mm, vSkip=0mm);
}
