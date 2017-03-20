import root;
import pad_layout;
import common_code;

string topDir = "../../";

string datasets[] = { "DS-fill5313" };

string objects[] = {
	"g_eff_45b",
	"g_eff_45t",
	"g_eff_comb1",
	"g_eff_comb2",
};


AddAllModes();

//----------------------------------------------------------------------------------------------------

NewPad(false, 0, -1);

AddToLegend("45 bot -- 56 top", StdPen(1));
AddToLegend("45 top -- 56 bot", StdPen(2));
AddToLegend("combined, mode 1", StdPen(3));
AddToLegend("combined, mode 2", StdPen(4));

AttachLegend();

//----------------------------------------------------------------------------------------------------

bool legend_drawn = false;
int idx = 0;

for (int dsi : datasets.keys)
{
	for (int xsi = 0; xsi < 2; ++xsi)
	{
		++idx;

		NewPad(false, idx, -1);
		string title = (xsi == 0) ? "low-$|t|$ zoom" : "full $|t|$ range";
		label("{\SetFontSizesXX "+replace("\vbox{\hbox{"+datasets[dsi]+"}\hbox{"+title+"}}", "_", "\_")+"}");

		for (int mi : modes.keys)
		{
			//write(modes[mi].tag);

			if (idx == 1)
			{
				NewPad(false, 0, mi);
				label("{\SetFontSizesXX " + modes[mi].label + "}");
			}
			
			NewPad("$|t|\ung{GeV^2}$", "systematic effect", idx, mi);
			currentpad.xSize = (xsi == 0) ? 6cm : 12cm;
		
			string f = topDir + "systematics/"+datasets[dsi]+"/matrix_numerical_integration.root";

			for (int oi : objects.keys)
			{
				string pth = "contributions/" + modes[mi].num_int_tag + "/" + objects[oi];
				RootObject obj = RootGetObject(f, pth, error=false);
				if (obj.valid)
					draw(obj, "l,d0", StdPen(oi+1)+1pt);
			}
			

			// ----- limits, grid, ... -----

			real x_min = 0, x_max = 0.3, x_Step = 0.05, x_step = 0.01;
			real y_min = -0.02, y_max = +0.02, y_Step = 0.005, y_step = 0.001;

			if (xsi == 0)
				{ x_min = 0; x_max = 0.005; x_Step = 0.002; x_step = 0.001; }

			currentpad.xTicks = LeftTicks(x_Step, x_step);
			currentpad.yTicks = RightTicks(y_Step, y_step);
		
			limits((0, y_min), (x_max, y_max), Crop);
			
			if (xsi == 0)
				yaxis(XEquals(8e-4, false), dashed);
		}
	}
}

GShipout(vSkip=0pt);
