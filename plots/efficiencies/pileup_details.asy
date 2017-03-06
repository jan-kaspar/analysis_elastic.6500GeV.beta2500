import root;
import pad_layout;

include "../run_info.asy";

string topDir = "../../";

string datasets[], dataset_fills[];
datasets.push("DS-fill5313"); dataset_fills.push("5313");
datasets.push("DS-fill5314"); dataset_fills.push("5314");
/*
datasets.push("DS-fill5317"); dataset_fills.push("5317");
datasets.push("DS-fill5321"); dataset_fills.push("5321");
*/

string diagonals[] = { "45b", "45t" };
string dgn_labels[] = { "45 bot -- 56 top", "45 top -- 56 bot" };

string elements[][] = {
	{ "L_2_F/#",  },
	{ "R_2_F/#",  },
	{ "dgn/#, L || R" }
};

string criteria[] = { "pat_suff", "pl_suff" };
//string criteria[] = { "pat_suff", "pl_suff", "tr_any" };

string row_captions[] = { "left", "right", "left OR right" };

string col_captions[] = { "220-far" };

xSizeDef = 15cm;

//----------------------------------------------------------------------------------------------------

TGraph_errorBar = None;

for (int dsi : datasets.keys)
{
	string f = topDir + datasets[dsi]+"/pileup_combined.root";

	for (int di : diagonals.keys)
	{
		string dgn = diagonals[di];
		
		NewPage();

		NewPad(false, -1, -1);
		label("\vbox{\SetFontSizesXX\hbox{"+datasets[dsi]+"}\hbox{"+dgn_labels[di]+"}}");
	
		for (int ri : row_captions.keys)
		{
			NewPad(false, -1, ri);
			label("{\SetFontSizesXX "+row_captions[ri]+"}");
		}
	
		for (int ci : col_captions.keys)
		{
			NewPad(false, ci, -1);
			label("{\SetFontSizesXX "+col_captions[ci]+"}");
		}

		frame fr_leg;
	
		for (int r = 0; r < elements.length; ++r)
		{
			for (int c = 0; c < elements[0].length; ++c)
			{
				string template = elements[r][c];
				if (template == "")
					continue;

				real y_max = 0.1;
		
				NewPad("time$\ung{h}$", "destructive pile-up probability", c, r);
				DrawRunBands(dataset_fills[dsi], 0, y_max);
		
				for (int ci : criteria.keys)
				{
					string element = replace(template, "#", criteria[ci]);
					pen p = StdPen(ci);
					draw(swToHours, RootGetObject(f, dgn+"/"+element+"/rel", search=false), "p", p, mCi+2pt+p, replace(criteria[ci], "_", "\_"));
				}
				
				ylimits(0, y_max, Crop);
				//limits((2, 0), (3.6, y_max), Crop);
				
				fr_leg = BuildLegend();
					
			}
		}

		NewPad(false, elements[0].length, elements.length - 1);
		attach(fr_leg);
	}
}

GShipout(hSkip=1mm);
