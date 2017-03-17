import root;
import pad_layout;
include "../run_info.asy";

string topDir = "../../";

string datasets[], dataset_fills[];
datasets.push("DS-fill5313"); dataset_fills.push("5313");
datasets.push("DS-fill5314"); dataset_fills.push("5314");
datasets.push("DS-fill5317"); dataset_fills.push("5317");
datasets.push("DS-fill5321"); dataset_fills.push("5321");

string diagonals[], diagonal_labels[];
diagonals.push("45b_56t"); diagonal_labels.push("45 bot -- 56 top");
diagonals.push("45t_56b"); diagonal_labels.push("45 top -- 56 bot");

xTicksDef = LeftTicks(Step = 1, step = 6);

//----------------------------------------------------------------------------------------------------

void SetPadWidth()
{
	real factorHoursToSize = 4cm / 3;

	real timespan = currentpicture.userMax2().x - currentpicture.userMin2().x;
	currentpad.xSize = timespan * factorHoursToSize;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

NewPad(false);
label("\vbox{\SetFontSizesXX\hbox{$\th_x^*$}\hbox{right -- left difference}}");

for (int dsi : datasets.keys)
{
	NewPad("time $\ung{h}$", "$\De^{R-L} \th_x^*\ung{\mu rad}$");
	real y_min = -1.5, y_max = 1.5;

	DrawRunBands(dataset_fills[dsi], y_min, y_max);

	for (int dgni : diagonals.keys)
	{
		string f = topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root";
		draw(scale(1/3600, 1e6), RootGetObject(f, "time dependences/p_diffLR_th_x_vs_time"), "eb,d0", StdPen(dgni+1));

		draw(scale(1/3600, 1e6), RootGetObject(f, "time dependences/g_ext_diffLR_th_x_vs_time"), "l", StdPen(dgni+1));
	}

	ylimits(y_min, y_max, Crop);

	SetPadWidth();
}


//----------------------------------------------------------------------------------------------------
NewRow();

NewPad(false);
label("\vbox{\SetFontSizesXX\hbox{$\th_y^*$}\hbox{right -- left difference}\hbox{extrapolated to $\th_y^* = 0$}}");

TGraph_errorBar = None;
for (int dsi : datasets.keys)
{
	NewPad("time $\ung{h}$", "$\De^{R-L} \th_y^{*}\ung{\mu rad}$");
	real y_min = -0.08, y_max = 0.08;

	DrawRunBands(dataset_fills[dsi], y_min, y_max);

	for (int dgni : diagonals.keys)
	{
		string f = topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root";
		draw(scale(1/3600, 1e6), RootGetObject(f, "time dependences/g_ext_diffLR_th_y_vs_time"), "p,l", StdPen(dgni+1), mCi+2pt+StdPen(dgni+1));
	}

	//ylimits(y_min, y_max, Crop);

	SetPadWidth();
}
