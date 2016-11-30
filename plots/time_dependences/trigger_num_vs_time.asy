import root;
import pad_layout;
include "../run_info.asy";

string datasets[], fills[];
datasets.push("DS-fill5313"); fills.push("5313");
datasets.push("DS-fill5314"); fills.push("5314");
datasets.push("DS-fill5317"); fills.push("5317");
datasets.push("DS-fill5321"); fills.push("5321");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };
pen dgn_pens[] = { blue, red };

string topDir = "../../";

xSizeDef = 10cm;
ySizeDef = 6cm;

xTicksDef = LeftTicks(1., 0.5);

TGraph_errorBar = None;

TGraph_reducePoints = 1000;

//----------------------------------------------------------------------------------------------------

void SetPadWidth()
{
	real factorHoursToSize = 6cm / 3;

	real timespan = currentpicture.userMax2().x - currentpicture.userMin2().x;
	currentpad.xSize = timespan * factorHoursToSize;
}

//----------------------------------------------------------------------------------------------------

void DrawQuick(RootObject obj, pen p, string label)
{
	int N = obj.iExec("GetN");

	for (int i = 0; i < N; i += 500)
	{
		real[] x = {0};
		real[] y = {0};
		obj.vExec("GetPoint", i, x, y);	

		draw(swToHours * (x[0], y[0]), mCi+1pt+p);
	}
}

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	NewRow();

	NewPad(false);
	label("{\SetFontSizesXX " + datasets[dsi] + "}");

	NewPad("time $\ung{h}$", "event number");
	//currentpad.yTicks = RightTicks(1., 0.2);
	real y_min = 3.7e9, y_max = 4.1e9;
	
	DrawRunBands(fills[dsi], y_min, y_max);

	for (int dgni : diagonals.keys)
	{
		DrawQuick(RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "metadata/g_tr_num_vs_timestamp"),
			dgn_pens[dgni], dgn_labels[dgni]);	
	}

	//ylimits(y_min, y_max, Crop);
	SetPadWidth();
}
