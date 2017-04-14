import root;
import pad_layout;

string f = "../../reconstruction_formulae/test_formulae_graph.root";

void DrawSet(string iq, real scale_x, string unit, string formulae[], string labels[])
{
	string complementary = (iq == "x") ? "angle" : "vertex";

	for (int fi : formulae.keys)
	{
		string formula = formulae[fi];
	
		NewRow();
	
		NewPad(false);
		label("{\SetFontSizesXX " + labels[fi] + "}");
		
		NewPad("$"+iq+"^{*, sim}\ung{"+unit+"}$", "std.~dev.~of $"+iq+"^{*, reco} - "+iq+"^{*, sim}\ung{"+unit+"}$");
		scale(Linear, Linear(true));
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/pitch/g_stddev"), "l,p", black, mCi+1pt+black, "pitch");
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/beamDiv/g_stddev"), "l,p", red, mCi+1pt+red, "beamDiv");
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/"+complementary+"/g_stddev"), "l,p", blue, mCi+1pt+blue, ""+complementary+"");
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/pitch,beamDiv,"+complementary+"/g_stddev"), "l,p", magenta, mCi+1pt+magenta, "pitch,beamDiv,"+complementary+"");
		picture prev_pic = currentpicture;
		
		/*
		NewPad("$"+iq+"^{*, sim}\ung{"+unit+"}$", "std.~dev.~of $"+iq+"^{*, reco} - "+iq+"^{*, sim}\ung{\mu rad}$");
		currentpicture.legend = prev_pic.legend;
		scale(Linear, Linear(true));
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/optics/g_stddev"), "l,p", heavygreen, mCi+1pt+heavygreen, "optics");
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/"+complementary+",optics/g_stddev"), "l,p", brown, mCi+1pt+brown, ""+complementary+",optics");
		prev_pic = currentpicture;
		
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
//----------------------------------------------------------------------------------------------------

string formulae[], labels[];
formulae.push("theta_x_one_arm_regr, th_x_L"); labels.push("\vbox{\hbox{reco for cuts}\hbox{$\th_x^*$}\hbox{single arm (45)}}");
formulae.push("theta_x_one_arm_regr_LRavg, th_x"); labels.push("\vbox{\hbox{reco for cuts}\hbox{$\th_x^*$}\hbox{two arms}}");

formulae.push("theta_x_one_arm_hit, th_x_L"); labels.push("\vbox{\hbox{reco for distributions}\hbox{$\th_x^*$}\hbox{single arm (45)}}");
formulae.push("theta_x_two_arm_hit_LRavg, th_x"); labels.push("\vbox{\hbox{reco for distributions}\hbox{$\th_x^*$}\hbox{two arms}}");

DrawSet("\th_x", 1e6, "\mu rad", formulae, labels);

GShipout("plot_formulae_graphs_desc_th_x");

//----------------------------------------------------------------------------------------------------

string formulae[], labels[];
formulae.push("theta_y_one_arm_hit, th_y_L"); labels.push("\vbox{\hbox{$\th_y^*$}\hbox{single arm (45)}}");
formulae.push("theta_y_two_arm_hit_LRavg, th_y"); labels.push("\vbox{\hbox{$\th_y^*$}\hbox{two arms}}");

DrawSet("\th_y", 1e6, "\mu rad", formulae, labels);

GShipout("plot_formulae_graphs_desc_th_y");
