import root;
import pad_layout;
include "../run_info.asy";

string topDir = "../../";

string datasets[], fills[];
datasets.push("DS-fill5313"); fills.push("5313");
datasets.push("DS-fill5314"); fills.push("5314");
datasets.push("DS-fill5317"); fills.push("5317");
datasets.push("DS-fill5321"); fills.push("5321");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };
pen dgn_pens[] = { blue, red };

string cuts[], c_units[];
real c_scales[];
cuts.push("1"); c_units.push("\mu rad"); c_scales.push(1e6);
cuts.push("2"); c_units.push("\mu rad"); c_scales.push(1e6);

string quantities[], q_options[], q_labels[];
quantities.push("p_cq_time"); q_options.push("eb,d0"); q_labels.push("mean vs.~time");
quantities.push("g_cq_RMS"); q_options.push("p,d0"); q_labels.push("RMS vs.~time");

xSizeDef = 10cm;
ySizeDef = 6cm;

xTicksDef = LeftTicks(1., 0.5);

TGraph_errorBar = None;

//TGraph_reducePoints = 10;

//----------------------------------------------------------------------------------------------------

void SetPadWidth()
{
	real factorHoursToSize = 6cm / 3;

	real timespan = currentpicture.userMax2().x - currentpicture.userMin2().x;
	currentpad.xSize = timespan * factorHoursToSize;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

for (int ci : cuts.keys)
{
	for (int qi : quantities.keys)
	{
		NewRow();

		NewPad(false);
		label("{\SetFontSizesXX\vtop{\hbox{cut " + cuts[ci] + ":}\hbox{" + q_labels[qi] + "}}}");

		for (int dsi : datasets.keys)
		{
			NewPad("time $\ung{h}$", "$\ung{" + c_units[ci] + "}$");
			//currentpad.yTicks = RightTicks(1., 0.2);
			//real y_min = -3, y_max = +2;
			
			//DrawRunBands(fills[dsi], y_min, y_max);
		
			for (int dgni : diagonals.keys)
			{
				RootObject obj = RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root",
					"elastic cuts/cut " + cuts[ci] + "/" + quantities[qi] + cuts[ci]);

				if (obj.InheritsFrom("TGraph"))
					draw(swToHours*scale(1, c_scales[ci]), obj, q_options[qi], dgn_pens[dgni], mCi+2pt+dgn_pens[dgni], dgn_labels[dgni]);
				else
					draw(swToHours*scale(1, c_scales[ci]), obj, q_options[qi], dgn_pens[dgni], dgn_labels[dgni]);
		
			}
		
			//ylimits(y_min, y_max, Crop);
			SetPadWidth();
		}
	}
}


//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

frame f_leg = BuildLegend();

NewRow();

NewPad(false);
attach(f_leg);


GShipout(vSkip=0mm);
