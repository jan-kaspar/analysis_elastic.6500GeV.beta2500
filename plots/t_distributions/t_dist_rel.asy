import root;
import pad_layout;
include common_code;

//----------------------------------------------------------------------------------------------------

string datasets[] = { "merged" };

//string diagonals[] = { "45b_56t", "45t_56b", "combined" };
//string diagonals_long[] = { "45 bottom - 56 top", "45 top - 56 bottom", "combined" };

//string diagonals[] = { "45t_56b" };
//string diagonals_long[] = { "45 top - 56 bottom" };

string diagonals[] = { "combined" };
string diagonals_long[] = { "combined" };

string topDir = "../../";

xSizeDef = 8cm;
ySizeDef = 6cm;

A_ref = 3.1e8;
B_ref = 20.6;
ref_str = MakeRefStr("");

//TH1_x_min = 0.04;

//----------------------------------------------------------------------------------------------------

frame fLegend;

void DrawSet(string binning)
{
	for (int di : diagonals.keys)
	{
		NewPad("$|t|\ung{GeV^2}$", "${\d\si/\d t - \hbox{ref} \over \hbox{ref}}\ ,\ \ \hbox{ref} = $ "+ref_str+"");
		currentpad.xTicks = LeftTicks(0.05, 0.01);
	
		for (int dsi : datasets.keys)
		{
			string dataset = datasets[dsi];
	
			pen p = StdPen(dsi+1);
			if (dataset == "DS4")
				p += 2pt;

			DrawRelDiff(
				RootGetObject(topDir+"DS-merged/merged.root", binning+"/"+datasets[dsi]+"/"+diagonals[di]+"/h_dsdt"),
				p, datasets[dsi]);
			
			limits((0, -0.02), (0.22, 0.01), Crop);
			xaxis(YEquals(0, false), dotted);
		}
	
		fLegend = BuildLegend("binning: " + binning);
		currentpicture.legend.delete();
		//AttachLegend("diagonal: " + diagonals_long[di], NW, NW);
		//AttachLegend("$\sqrt s = 13\un{TeV}$");
	}
		
	//NewPad(false);
	//add(fLegend);
}

//----------------------------------------------------------------------------------------------------

DrawSet("ob-1-30-0.05");
