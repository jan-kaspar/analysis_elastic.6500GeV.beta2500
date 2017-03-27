import root;
import pad_layout;

string topDir = "../../../acceptance/";

string th_x_strs[] = {
	"-5.0E-05",
	"+0.0E+00",
	"+5.0E-05",
};

string f = topDir + "validation_vs_old.root";

//----------------------------------------------------------------------------------------------------

for (string th_x_str : th_x_strs)
{
	NewPad("$\th_y^*\ung{\mu rad}$", "$A_{\rm sm}(\th_x^*, \th_y^*)$");
	currentpad.xTicks = LeftTicks(0.1, 0.05);

	draw(scale(1e6, 1.), RootGetObject(f, "g_old_A_th_x_" + th_x_str), blue);
	draw(scale(1e6, 1.), RootGetObject(f, "g_new_A_th_x_" + th_x_str), red+dashed+1pt);

	limits((4, 0), (5, 1), Crop);

	AttachLegend("$\th_x^* = $" + th_x_str, NW, NW);
}

//----------------------------------------------------------------------------------------------------

NewRow();

NewPad("$|t|\ung{GeV^2}$", "$A_\ph(t)$");
draw(RootGetObject(f, "g_A_t_old"), blue, "old");
draw(RootGetObject(f, "g_A_t_new"), red+dashed+1pt, "new");

limits((0, 0), (0.5, 0.5), Crop);

frame f_legend = BuildLegend();

NewPad(false);
add(f_legend);
