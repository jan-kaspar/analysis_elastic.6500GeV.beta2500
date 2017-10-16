import root;
import pad_layout;

string f = "../../reconstruction_formulae/test_formulae_graph.root";

void DrawSet(string iq, real scale_x, string unit, string formulae[])
{
	string complementary = (iq == "x") ? "angle" : "vertex";

	for (int fi : formulae.keys)
	{
		string formula = formulae[fi];
	
		NewRow();
	
		NewPad(false);
		label("{\SetFontSizesXX " + replace(formula, "_", "\_") + "}");
		
		NewPad("$"+iq+"^{*,\rm sim}\ung{"+unit+"}$", "std.~dev.~of $"+iq+"^{*,\rm reco} - "+iq+"^{*,\rm sim}\ung{"+unit+"}$");
		scale(Linear, Linear(true));
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/pitch/g_stddev"), "l,p", black, mCi+1pt+black, "pitch");
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/beamDiv/g_stddev"), "l,p", red, mCi+1pt+red, "beamDiv");
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/"+complementary+"/g_stddev"), "l,p", blue, mCi+1pt+blue, ""+complementary+"");
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/pitch,beamDiv,"+complementary+"/g_stddev"), "l,p", magenta, mCi+1pt+magenta, "pitch,beamDiv,"+complementary+"");
		picture prev_pic = currentpicture;
		
		NewPad("$"+iq+"^{*, sim}\ung{"+unit+"}$", "std.~dev.~of $"+iq+"^{*, reco} - "+iq+"^{*, sim}\ung{\mu rad}$");
		currentpicture.legend = prev_pic.legend;
		scale(Linear, Linear(true));
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/optics/g_stddev"), "l,p", heavygreen, mCi+1pt+heavygreen, "optics");
		//draw(scale(scale_x, 1), RootGetObject(f, formula+"/"+complementary+",optics/g_stddev"), "l,p", brown, mCi+1pt+brown, ""+complementary+",optics");
		prev_pic = currentpicture;
		
		/*
		NewPad("$"+iq+"^{*, sim}\ung{"+unit+"}$", "std.~dev.~of $"+iq+"^{*, reco} - "+iq+"^{*, sim}\ung{\mu rad}$");
		currentpicture.legend = prev_pic.legend;
		scale(Linear, Linear(true));
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/pitch,beamDiv,"+complementary+",optics/g_stddev"), "l,p", orange+1pt, mCi+1.5pt+orange, "pitch,beamDiv,"+complementary+",optics");
		*/

		frame lf = BuildLegend();

		NewPad(false);
		attach(lf);
	}
}

//----------------------------------------------------------------------------------------------------

string formulae[] = {
	"theta_x_one_pot_hit_L2F, th_x_L",
	"theta_x_one_pot_hit_L1F, th_x_L",
//	"theta_x_one_pot_hit_R1F, th_x_R",
//	"theta_x_one_pot_hit_R2F, th_x_R",
};

DrawSet("\th_x", 1e6, "\mu rad", formulae);

//----------------------------------------------------------------------------------------------------
NewPage();

string formulae[] = {
	"theta_x_one_arm_hit, th_x_L",
	"theta_x_one_arm_angle, th_x_L",
	"theta_x_one_arm_regr, th_x_L",
};

DrawSet("\th_x", 1e6, "\mu rad", formulae);

//----------------------------------------------------------------------------------------------------
NewPage();

string formulae[] = {
	"theta_x_two_arm_hit_2F_LRavg, th_x",
	"theta_x_two_arm_hit_LRavg, th_x",
	"theta_x_two_arm_angle_LRavg, th_x",
	"theta_x_one_arm_regr_LRavg, th_x",
	"theta_x_two_arm_full_regr, th_x"
};

DrawSet("\th_x", 1e6, "\mu rad", formulae);

//----------------------------------------------------------------------------------------------------
NewPage();

string formulae[] = {
	"theta_y_one_pot_hit_L2F, th_y_L",
	"theta_y_one_pot_hit_L1F, th_y_L",
//	"theta_y_one_pot_hit_R1F, th_y_R",
//	"theta_y_one_pot_hit_R2F, th_y_R",
};

DrawSet("\th_y", 1e6, "\mu rad", formulae);

//----------------------------------------------------------------------------------------------------
NewPage();

string formulae[] = {
	"theta_y_one_arm_hit, th_y_L",
	"theta_y_one_arm_angle, th_y_L",
	"theta_y_one_arm_regr, th_y_L",
};

DrawSet("\th_y", 1e6, "\mu rad", formulae);

//----------------------------------------------------------------------------------------------------
NewPage();
	
string formulae[] = {
	"theta_y_two_arm_hit_2F_LRavg, th_y",
	"theta_y_two_arm_hit_LRavg, th_y",
	"theta_y_two_arm_angle_LRavg, th_y",
	"theta_y_two_arm_simple_regr, th_y",
};

DrawSet("\th_y", 1e6, "\mu rad", formulae);

//----------------------------------------------------------------------------------------------------
NewPage();

string formulae[] = {
	"vtx_x_one_arm_regr, vtx_x_L",
};

DrawSet("x", 1e3, "\mu m", formulae);

//----------------------------------------------------------------------------------------------------
NewPage();

string formulae[] = {
	"vtx_x_one_arm_regr_LRavg, vtx_x",
	"vtx_x_two_arm_full_regr, vtx_x",

};

DrawSet("x", 1e3, "\mu m", formulae);
