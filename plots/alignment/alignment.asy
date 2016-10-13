import root;
import pad_layout;
include "../run_info.asy";

string topDir = "../../";

string datasets[];
real times_min[], times_max[];

datasets.push("DS-fill5313"); //times_min.push(5); times_max.push(10);
datasets.push("DS-fill5314"); //times_min.push(5); times_max.push(10);
datasets.push("DS-fill5317"); //times_min.push(5); times_max.push(10);
datasets.push("DS-fill5321"); //times_min.push(5); times_max.push(10);

string units[] = { "L_2_F", "L_1_F", "R_1_F", "R_2_F" };
string unit_labels[] = { "L-220-fr", "L-210-fr", "R-210-rf", "R-220-fr" };

//string units[] = { "L_F", "L_N" };
//string unit_labels[] = { "Left Far", "Left Near" };

xSizeDef = 10cm;
xTicksDef = LeftTicks(Step=0.5, step=0.25);
drawGridDef = true;

TGraph_errorBar = None;

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	string dataset = datasets[dsi];
	//real time_min = times_min[dsi];
	//real time_max = times_max[dsi];

	write(dataset);

	for (int ui : units.keys)
	{
		NewPad("time $\ung{h}$", "tilt $\ung{mrad}$", axesAbove=false);
		//currentpad.yTicks = RightTicks(0.2, 0.1);
		DrawRunBands(dataset, -1, +1);
	
			draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/a_p"), "p,eb", cyan, mCi+1pt+cyan);
			draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/a_g"), "p,eb", green, mCi+1pt+green);
			
			draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/a"), "p,eb", blue, mCi+1pt+blue);
		
		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/a_fit"), "l", red+1.5pt);
	
		//limits((time_min, -1), (time_max, +1), Crop);
		AttachLegend(unit_labels[ui], SE, SE);
	}
	
	//----------------------------------------------------------------------------------------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("time $\ung{h}$", "horizontal position $\ung{\mu m}$", axesAbove=false);
		currentpad.yTicks = RightTicks(20., 10.);
		DrawRunBands(dataset, -80, +80);
	
		/*
		TGraph_reducePoints = 30;
		draw(unixToHours * shift(0, sh_x[ui]), RootGetObject("bpm.root", "LHC.BOFSU:POSITIONS_H::"+bpms[ui]), black);
		TGraph_reducePoints = 1; 
		*/
	
			draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/b_p"), "p,eb", cyan, mCi+1pt+cyan);
			draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/b_g"), "p,eb", green, mCi+1pt+green);
	
			draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/b"), "p,eb", blue+1pt, mCi+1pt+blue);
		
		//draw(shift(0,   0)*swToHours, RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/b_fit"), "l", red+1.5pt);
	
		/*
		if (dataset == "DS1")
			limits((time_min, -140), (time_max, -60), Crop);
		else
			limits((time_min, -60), (time_max, +60), Crop);
		*/
		AttachLegend(unit_labels[ui], SE, SE);
	}
	
	//----------------------------------------------------------------------------------------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("time $\ung{h}$", "vertical position $\ung{\mu m}$", axesAbove=false);
		currentpad.yTicks = RightTicks(50., 10.);
		DrawRunBands(dataset, -200, +100);
	
		/*
		TGraph_reducePoints = 30;
		draw(unixToHours*shift(0, sh_y[ui]), RootGetObject("bpm.root", "LHC.BOFSU:POSITIONS_V::"+bpms[ui]), black);
		TGraph_reducePoints = 1;
		*/
	
			draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_min_diff"), "p,eb", cyan, mCi+1pt+cyan);
			draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_prob"), "p,eb", blue, mCi+1pt+blue);
			draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_mean_diff_sq"), "p,eb", magenta, mCi+1pt+magenta);
			draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_hist_chi_sq"), "p,eb", heavygreen, mCi+1pt+heavygreen);
			
			//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c"), "p,l,eb", blue+1pt, mCi+1pt+blue);
		
		//draw(swToHours*shift(0,    0), RootGetObject(topDir+dataset+"/alignment_fit.root", ""+units[ui]+"/c_fit"), "l", red+1.5pt);
	
		/*
		if (dataset == "DS1")
			limits((time_min, -400), (time_max, -100), Crop);
		else
			limits((time_min, -200), (time_max, +100), Crop);
		*/
		AttachLegend(unit_labels[ui], SE, SE);
	}
	
	//----------------------------------------------------------------------------------------------------
	
	GShipout("alignment_" + dataset);
}
