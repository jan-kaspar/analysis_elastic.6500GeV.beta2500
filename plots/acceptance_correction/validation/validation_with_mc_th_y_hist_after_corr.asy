import root;
import pad_layout;

string topDir = "../../../acceptance/";

string rows[] = {
	"simulations/0.08,1E7,uncorr,old,flat/seed1/validation_with_mc.root",
	"simulations/0.08,1E7,uncorr,new,curved/seed1/validation_with_mc.root",

	"simulations/0.08,1E7,corr,old,flat/seed1/validation_with_mc.root",
	"simulations/0.08,1E7,corr,new,curved/seed1/validation_with_mc.root",
};

string th_x_ranges[] = {
	"-3.1E-05_-3.0E-05",
	"-5.0E-07_+5.0E-07", 
	"+3.0E-05_+3.1E-05", 
};

//----------------------------------------------------------------------------------------------------

NewPad(false);

for (string th_x_range : th_x_ranges)
{
	NewPad(false);
	label("$\th_x^*$ from " + replace(th_x_range, "_", " to "));
}

for (string row : rows)
{
	NewRow();

	NewPad(false);
	label(replace(row, "_", "\_"));

	string f = topDir + row;

	for (string th_x_range : th_x_ranges)
	{
		string dir = "th_x_ranges/" + th_x_range + "/";

		NewPad("$\theta^*_y\ung{\mu rad}$");
		draw(scale(1e6, 1), RootGetObject(f, dir + "h_th_ratio_cut_corr_over_no_cut"), "eb", magenta);
		
		limits((4, 0.8), (6, 1.1), Crop);
	
		xaxis(YEquals(1, false), dashed);
	}
}

GShipout(vSkip=1mm);
