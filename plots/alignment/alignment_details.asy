import root;
import pad_layout;
include "../run_info.asy";

string topDir = "../../";


string datasets[];
string periods[];

datasets.push("DS1"); periods.push("70");
datasets.push("DS2"); periods.push("177");
datasets.push("DS3"); periods.push("293");
datasets.push("DS4"); periods.push("565");
datasets.push("DS5"); periods.push("734");
datasets.push("DS6"); periods.push("842");
datasets.push("DS7"); periods.push("970");

string units[] = { "L_2_F", "L_2_N", "L_1_F", "R_1_F", "R_2_N", "R_2_F" };
string unit_labels[] = { "left, 220, far", "left, 220, near", "left, 210, far", "right, 210, far", "right, 220, near", "right, 220, far" };

//string units[] = { "L_2_F", "L_2_N" };
//string unit_labels[] = { "Left Far", "Left Near" };

xSizeDef = 10cm;
drawGridDef = true;

TGraph_errorBar = None;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	string dataset = datasets[dsi];
	string period = "period " + periods[dsi];

	write(dataset);

	//--------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("$y\ung{mm}$", "$\hbox{mean } x\ung{mm}$");
	
		draw(rGetObj(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/horizontal/horizontal profile/p"), "d0,eb", blue);
		draw(rGetObj(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/horizontal/horizontal profile/p|ff"), "l", red+1pt);
		
		limits((-30, -0.2), (+30, +0.2), Crop);
		AttachLegend(unit_labels[ui], NE, NE);
	}
	
	//--------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("$y\ung{mm}$", "");
		scale(Linear, Log);
	
		draw(rGetObj(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/vertical/y_hist|y_hist"), "d0,vl", blue);
		draw(rGetObj(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/vertical/y_hist|y_hist_range"), "d0,vl", red+1pt);
	
		limits((-30, 1), (+30, 5e4), Crop);
		AttachLegend(unit_labels[ui], NE, NE);
	}
	
	//--------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("bottom-RP $y$ shift$\ung{mm}$", "");
		currentpad.xTicks = LeftTicks(0.5, 0.1);
	
		draw(rGetObj(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/vertical/g_max_diff"), "l,p", heavygreen, mCi+1pt+heavygreen);
	
		limits((-2.0, 0), (+2.0, 0.2), Crop);
		AttachLegend(unit_labels[ui], NE, NE);
	}

	//--------------------
	
	GShipout("alignment_details_" + dataset);
}
