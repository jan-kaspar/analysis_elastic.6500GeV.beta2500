import root;
import pad_layout;
include "../run_info.asy";

string topDir = "../../";


string datasets[];
string periods[];
datasets.push("DS-fill5313"); periods.push("2"); 		// 1 to 26
//datasets.push("DS-fill5314"); periods.push("43");		// 42 to
//datasets.push("DS-fill5317"); periods.push("88");		// 87 to
//datasets.push("DS-fill5321"); periods.push("221");	// 221 to

string units[], unit_labels[];
units.push("L_2_F"); unit_labels.push("L-220-fr");
units.push("L_1_F"); unit_labels.push("L-210-fr");
units.push("R_1_F"); unit_labels.push("R-210-rf");
units.push("R_2_F"); unit_labels.push("R-220-fr" );

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
	
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/horizontal/horizontal profile/p"), "d0,eb", blue);
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/horizontal/horizontal profile/p|ff"), "l", red+1pt);
		
		limits((-30, -0.5), (+30, +0.5), Crop);
		AttachLegend(unit_labels[ui], NE, NE);
	}
	
	//--------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("$y\ung{mm}$", "");
		scale(Linear, Log);
	
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/vertical/y_hist|y_hist"), "d0,vl", blue);
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/vertical/y_hist|y_hist_range"), "d0,vl", red+1pt);
	
		limits((-30, 1), (+30, 1e3), Crop);
		AttachLegend(unit_labels[ui], NE, NE);
	}
	
	//--------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("bottom-RP $y$ shift$\ung{mm}$", "");
		currentpad.xTicks = LeftTicks(0.5, 0.1);
	
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/vertical/g_max_diff"), "l,p", heavygreen, mCi+1pt+heavygreen);
	
		limits((-1.0, 0), (+1.0, 0.2), Crop);
		AttachLegend(unit_labels[ui], NE, NE);
	}

	//--------------------
	
	GShipout("alignment_details_" + dataset);
}
