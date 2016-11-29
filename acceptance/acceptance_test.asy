import root;
import pad_layout;

string th_x_strs[] = {
	"-5.0E-05",
	"+0.0E+00",
	"+5.0E-05",
};

string f = "acceptance_test.root";

//----------------------------------------------------------------------------------------------------

for (string th_x_str : th_x_strs)
{
	NewPad("$\th_y^*\ung{\mu rad}$", "$A_{\rm sm}(\th_x^*, \th_y^*)$");
	draw(scale(1e6, 1.), RootGetObject(f, "g_old_A_th_x_" + th_x_str), blue);
	draw(scale(1e6, 1.), RootGetObject(f, "g_new_A_th_x_" + th_x_str), red+dashed);
	AttachLegend(th_x_str, NW, NW);
}

//----------------------------------------------------------------------------------------------------

NewRow();

NewPad("$|t|\ung{GeV^2}$", "$A_\ph(t)$");
draw(RootGetObject(f, "g_A_t_old"), blue);
draw(RootGetObject(f, "g_A_t_new"), red+dashed);

