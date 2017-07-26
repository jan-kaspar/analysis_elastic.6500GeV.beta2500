import root;
import pad_layout;

string topDir = "../../";

string dataset = "DS-fill5314";

string dgns[] = { "45b_56t", "45t_56b" };

int cuts[] = { 1, 2, 5, 6, 7 };

real scale_x[] = { 1e6, 1e6, 1e6, 1e6, 1e0, 1e0, 1e6, 1e6 };
real scale_y[] = { 1e6, 1e6, 1e0, 1e0, 1e0, 1e0, 1e0, 1e0 };

string label_x[] = { "$\th_x^{*R}\ung{\mu rad}$", "$\th_y^{*R}\ung{\mu rad}$", "$\th_x^{*R}\ung{\mu rad}$", "$\th_x^{*L}\ung{\mu rad}$", "$y^{R,210,F}\ung{mm}$", "$y^{L,210,F}\ung{mm}$", "$\th_x^*\ung{\mu rad}$", "$\th_y^*\ung{\mu rad}$" };
string label_y[] = { "$\th_x^{*L}\ung{\mu rad}$", "$\th_y^{*L}\ung{\mu rad}$", "$x^{*R}\ung{mm}$", "$x^{*L}\ung{mm}$", "$y^{R,220,F} - y^{R,210,F}\ung{mm}$", "$y^{L,220,F} - y^{L,210,F}\ung{mm}$", "$\De^{R-L} x^*\ung{mm}$", "$\De^{R-L} y^*\ung{mm}$" };
string label_cut[] = { "$\De^{R-L} \th_x^{*}\ung{\mu rad}$", "$\De^{R-L} \th_y^{*}\ung{\mu rad}$", "$x^{*R}\ung{mm}$", "$x^{*L}\ung{mm}$", "$cq5$", "$cq6$", "$cq7$", "$cq8$" };

real lim_x_low[] = { -200, -150, -1000, -1000, -30, -30, -200, -600 };
real lim_x_high[] = { +200, 150, +1000, +1000, +30, +30, +200, +600 };

real lim_y_low[] = { -200, -150, -0.8, -0.8, -5, -5, -5, -4 };
real lim_y_high[] = { +200, 150, +0.8, +0.8, +5, +5, +5, +4 };

real lim_q[] = { 25, 5, 10., 10., 0.15, 0.15, 2.5 };

//----------------------------------------------------------------------------------------------------

NewPad(false);
label(replace("{\SetFontSizesXX " + dataset + "}", "_", "\_"));

NewRow();

NewPad(false);
for (string dgn : dgns)
{
	NewPad(false);
	label("{\SetFontSizesXX " + replace(dgn, "_", "\_") + "}");
}

for (int ci : cuts.keys)
{
	int cut = cuts[ci];
	int idx = cut - 1;

	NewRow();

	NewPad(false);
	label(format("{\SetFontSizesXX cut %u}", cut));

	for (int dgi : dgns.keys)
	{
		string dgn = dgns[dgi];
		string f = topDir + dataset+"/distributions_" + dgn + ".root";
	
		// ---------- discriminator distribution ----------

		string obj_name_par = format("elastic cuts/cut %i", cut) + format("/g_cut_parameters", cut);
		RootObject obj_par = RootGetObject(f, obj_name_par);
		real ax[] = {0}, ay[] = {0};
		obj_par.vExec("GetPoint", 0, ax, ay); real cca = ay[0];
		obj_par.vExec("GetPoint", 1, ax, ay); real ccb = ay[0];
		obj_par.vExec("GetPoint", 2, ax, ay); real ccc = ay[0];
		obj_par.vExec("GetPoint", 3, ax, ay); real csi = ay[0];
		obj_par.vExec("GetPoint", 4, ax, ay); real n_si = ay[0];

		NewPad(label_cut[idx]);

		string obj_name_h = format("elastic cuts/cut %i", cut) + format("/h_cq%i", cut);
		RootObject obj_h = RootGetObject(f, obj_name_h);

		real scale = scale_y[idx];

		draw(scale(scale, 1.), obj_h, "vl,eb", red+1pt);

		xlimits(-lim_q[idx], +lim_q[idx], Crop);

		yaxis(XEquals(+n_si * csi * scale, false), blue+dashed);
		yaxis(XEquals(-n_si * csi * scale, false), blue+dashed);

		AddToLegend(format("<mean = $%#.3f$", obj_h.rExec("GetMean") * scale));
		AddToLegend(format("<RMS = $%#.3f$", obj_h.rExec("GetRMS") * scale));
		AddToLegend(format("<cut = $\pm%#.3f$", n_si * csi * scale));
		AttachLegend();
	}
}
