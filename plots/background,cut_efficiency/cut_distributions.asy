import root;
import pad_layout;

string datasets[];
//datasets.push("DS-fill5313");
//datasets.push("DS-fill5314");
datasets.push("DS-fill5317");
//datasets.push("DS-fill5321");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };

string topDir = "../../";

xSizeDef = 8cm;

//----------------------------------------------------------------------------------------------------

string dataset;
string diagonal;

void MakeComparison(string quantity, real xscale, string unit, string obj, real xlimit, real sigma,
	real xStep, real xstep,
	string combinations[], pen comb_pens[])
{
	NewPad(quantity+"$\ung{"+unit+"}$", "", xTicks=LeftTicks(xStep, xstep));
	scale(Linear, Log(true));
	for (int ci : combinations.keys)
	{
		string f = topDir+"/background_studies/"+dataset+"/"+combinations[ci]+"/distributions_"+diagonal+".root";
		draw(scale(xscale, 1), RootGetObject(f, "elastic cuts/"+obj), "vl",
			comb_pens[ci], replace(combinations[ci], "_", "\_"));	
	}

	yaxis(XEquals(-4*sigma, false), dashed);
	yaxis(XEquals(+4*sigma, false), dashed);

	xlimits(-xlimit, +xlimit, Crop);
	AttachLegend(quantity, NW, NE);
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	for (int dgi : diagonals.keys)
	{
		NewRow();

		dataset = datasets[dsi];
		diagonal = diagonals[dgi];

		NewPad(false);
		label(replace("{\SetFontSizesXX\vbox{\hbox{"+dataset+"}\hbox{"+dgn_labels[dgi]+"}}}", "_", "\_"));

		string combinations[];
		pen comb_pens[];
		
		combinations.push("no_cuts"); comb_pens.push(gray);
		combinations.push("cuts:2"); comb_pens.push(red);
		combinations.push("cuts:2,7"); comb_pens.push(blue);
		combinations.push("cuts:2,7,5"); comb_pens.push(heavygreen);
		combinations.push("cuts:2,7,5,6"); comb_pens.push(magenta);
		
		MakeComparison("$\De\th_x^*$", 1e6, "\mu rad", "cut 1/h_cq1", xlimit=80, sigma=2, xStep=20, xstep=10, combinations, comb_pens);
		
		//--------------------
		//NewPage();
		
		string combinations[];
		pen comb_pens[];
		
		combinations.push("no_cuts"); comb_pens.push(gray);
		combinations.push("cuts:1"); comb_pens.push(red);
		combinations.push("cuts:1,7"); comb_pens.push(blue);
		combinations.push("cuts:1,7,5"); comb_pens.push(heavygreen);
		combinations.push("cuts:1,7,5,6"); comb_pens.push(magenta);
		
		MakeComparison("$\De\th_y^*$", 1e6, "\mu rad", "cut 2/h_cq2", xlimit=15, sigma=0.39, xStep=5, xstep=1, combinations, comb_pens);
		
		//--------------------
		//NewPage();
		
		string combinations[];
		pen comb_pens[];
		
		combinations.push("no_cuts"); comb_pens.push(gray);
		combinations.push("cuts:1"); comb_pens.push(red);
		combinations.push("cuts:1,2"); comb_pens.push(blue);
		combinations.push("cuts:1,2,5"); comb_pens.push(heavygreen);
		combinations.push("cuts:1,2,5,6"); comb_pens.push(magenta);
		
		MakeComparison("$\De x^*$", 1., "mm", "cut 7/h_cq7", xlimit=10., sigma=0.2, xStep=2., xstep=1, combinations, comb_pens);
	}
}
