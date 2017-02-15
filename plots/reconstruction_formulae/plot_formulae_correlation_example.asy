import root;
import pad_layout;

string f = "../../reconstruction_formulae/test_formulae_correlation.root";

string simulations[];
simulations.push("pitch,beamDiv,vertex");

string cen_val = "th_x=5.00E-05,th_y=5.00E-05,vtx_x=0.00E+00";

//----------------------------------------------------------------------------------------------------

void DrawSet(string formula, real scale_x, real limit_x, string label_x, real scale_y, real limit_y, string label_y)
{
	for (int si : simulations.keys)
	{
		NewPad(label_x, label_y);
		//scale(Linear, Linear);
		
		draw(scale(scale_x, scale_y), RootGetObject(f, formula + "/" + simulations[si] + "/" + cen_val + "/h2_scatter"));
		
		limits((-limit_x, -limit_y), (limit_x, limit_y), Crop);
	}
}

//----------------------------------------------------------------------------------------------------

DrawSet("theta_x_one_pot_hit_LR2F", 1e6, 25, "$\De\th_x^{*,R}\ung{\mu rad}$", 1e6, 25, "$\De\th_x^{*,L}\ung{\mu rad}$");

DrawSet("theta_y_one_pot_hit_LR2F", 1e6, 1, "$\De\th_y^{*,R}\ung{\mu rad}$", 1e6, 1, "$\De\th_y^{*,L}\ung{\mu rad}$");
