import root;
import pad_layout;

string topDir = "../";

string f_fits = topDir + "make_fits.root";

string datasets[];
datasets.push("DS-fill5313");
//datasets.push("DS-fill5314");
datasets.push("DS-fill5317");
//datasets.push("DS-fill5321");

string diagonals[], dgn_labels[];
pen dgn_pens_data[], dgn_pens_fit[];
diagonals.push("45b_56t"); dgn_labels.push("45 bot -- 56 top"); dgn_pens_data.push(black); dgn_pens_fit.push(heavygreen);
diagonals.push("45t_56b"); dgn_labels.push("45 top -- 56 bot"); dgn_pens_data.push(blue); dgn_pens_fit.push(red);

string constraints[];
string c_q1s[], c_q2s[];
string c_rotations[][];
constraints.push("R1"); c_q1s.push("\th_x^{loc,R}\ung{rad}"); c_q2s.push("\th_x^{loc,L}\ung{rad}"); c_rotations.push(new string[] {"0.00000", "1.57080", "-0.78540"});
constraints.push("R2"); c_q1s.push("y^{R-220-fr}\ung{mm}"); c_q2s.push("y^{L-220-fr}\ung{mm}"); c_rotations.push(new string[] {"0.00000", "1.57080", "-0.78540"});

constraints.push("R3"); c_q1s.push("\th_y^{loc,R}\ung{rad}"); c_q2s.push("y^{R-220-fr}\ung{mm}"); c_rotations.push(new string[] {"0.00000", "1.57080", "1.57078"});
constraints.push("R4"); c_q1s.push("\th_y^{loc,L}\ung{rad}"); c_q2s.push("y^{L-220-fr}\ung{mm}"); c_rotations.push(new string[] {"0.00000", "1.57080", "1.57078"});

constraints.push("R5"); c_q1s.push("\th_x^{loc,R}\ung{rad}"); c_q2s.push("x^{R-220-fr}\ung{mm}"); c_rotations.push(new string[] {"0.00000", "1.57080", "-1.57073"});
constraints.push("R6"); c_q1s.push("\th_x^{loc,L}\ung{rad}"); c_q2s.push("x^{L-220-fr}\ung{mm}"); c_rotations.push(new string[] {"0.00000", "1.57080", "-1.57073"});

constraints.push("R7");  c_q1s.push("x^{L-220-fr}"); c_q2s.push("y^{L-220-fr}"); c_rotations.push(new string[] {"1.57080"});
constraints.push("R8");  c_q1s.push("x^{L-210-fr}"); c_q2s.push("y^{L-210-fr}"); c_rotations.push(new string[] {"1.57080"});
constraints.push("R9");  c_q1s.push("x^{R-210-fr}"); c_q2s.push("y^{R-210-fr}"); c_rotations.push(new string[] {"1.57080"});
constraints.push("R10"); c_q1s.push("x^{R-220-fr}"); c_q2s.push("y^{R-220-fr}"); c_rotations.push(new string[] {"1.57080"});

//----------------------------------------------------------------------------------------------------

for (int ci : constraints.keys)
{
	write("* " + constraints[ci]);

	NewPad(false);
	label("\vbox{\SetFontSizesXX\hbox{\bf constraint: " + constraints[ci] + "}\hbox{$q_1 = " + c_q1s[ci] + "$}\hbox{$q_2 = " + c_q2s[ci] + "$}" + 
		"\hbox{correlation: $q_1 = " + constraints[ci] + "\cdot q_2 + const.$}}");

	for (int dsi : datasets.keys)
	{
		for (int dgni : diagonals.keys)
		{
			NewRow();

			NewPad(false);
			label("\vbox{\SetFontSizesXX\hbox{" + datasets[dsi] + "}\hbox{" + dgn_labels[dgni] + "}}");

			for (string rotation : c_rotations[ci])
			{
				string path_base = datasets[dsi] + "/" + diagonals[dgni] + "/" + constraints[ci] + "/" + rotation;

				NewPad("$h \equiv \cos(\rh)\,q_1 + \sin(\rh)\,q_2$", "$v \equiv -\sin(\rh)\,q_1 + \cos(\rh)\,q_2$");

				RootObject profile = RootGetObject(f_fits, path_base + "/p");
				RootObject fit = RootGetObject(f_fits, path_base + "/p|ff");
				RootObject data = RootGetObject(f_fits, path_base + "/g_data");

				draw(fit, dgn_pens_fit[dgni]+1.5pt);
				draw(profile, "eb", dgn_pens_data[dgni]);

				AddToLegend("<$\rh = " + rotation + "$");
	
				real ax[] = {0.};
				real ay[] = {0.};

				data.vExec("GetPoint", 0, ax, ay);
				real al = ax[0], al_unc = ay[0];

				data.vExec("GetPoint", 1, ax, ay);
				real a = ax[0], a_unc = ay[0];

				AddToLegend("<" + constraints[ci] + " = " + format("%#.4E", a) + "$\pm$" + format("%#.1E", a_unc));

				AttachLegend();
			}
		}
	}
	
	GShipout("optics_constraint_" + constraints[ci]);
}
