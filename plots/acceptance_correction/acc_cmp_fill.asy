import root;
import pad_layout;

string top_dir = "../../";

string datasets[] = {
//	"DS-fill5313",
	"DS-fill5314",
	"DS-fill5317",
	"DS-fill5321",
};

TH2_palette = Gradient(blue, heavygreen, yellow, red);

//----------------------------------------------------------------------------------------------------

real cut_th_x_low_top = -250, cut_th_x_high_top = 250;
real cut_th_y_low_top = 5, cut_th_y_high_top = +100;

real cut_th_x_low_bot = -250, cut_th_x_high_bot = 250;
real cut_th_y_low_bot = -5, cut_th_y_high_bot = -100;

void DrawAcceptedArcs(real th)
{
	real dphi = 360. / 1000;
	bool inside = false;
	real phi_start;
	for (real phi = 0; phi < 360; phi += dphi)
	{
		real x = th * Cos(phi);
		real y = th * Sin(phi);

		bool p_in = false;
		if (x > cut_th_x_low_top && x < cut_th_x_high_top && y > cut_th_y_low_top && y < cut_th_y_high_top)
			p_in = true;
		if (x > cut_th_x_low_bot && x < cut_th_x_high_bot && y < cut_th_y_low_bot && y > cut_th_y_high_bot)
			p_in = true;

		if (!inside && p_in)
		{
			phi_start = phi;
			inside = true;
		}

		if (inside && !p_in)
		{
			inside = false;
			real phi_end = phi - dphi;
			draw(arc((0, 0), th, phi_start, phi_end), black+1pt);
		}
	}
}

//----------------------------------------------------------------------------------------------------

void DrawFullArc(real th)
{
	draw(scale(th)*unitcircle, dotted);
	label(rotate(-90)*Label(format("\SmallerFonts $%.0f$", th)), (th, 0), 0.5E, Fill(white+opacity(0.8)));
}

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	real ySize = 6cm;

	NewPad("$\th_x^{*}\ung{\mu rad}$", "$\th_y^{*}\ung{\mu rad}$", ySize/150*150, ySize);
	currentpad.xTicks = LeftTicks(50., 10.);
	scale(Linear, Linear, Log);
	//TH2_zLabel = "(corrected) events per bin";
	TH2_paletteBarWidth = 0.05;
	
	label("$\th^*\!=$", (50, 0), 0.5W, Fill(white+opacity(0.8)));
	DrawFullArc(50);
	DrawFullArc(100);
	DrawFullArc(150);
	label(rotate(-90)*Label("\SmallerFonts $\rm\mu rad$"), (165, 0), 0.5E, Fill(white+opacity(0.8)));

	// z scale
	//TH2_z_min = 5.5;
	//TH2_z_max = 3.75;

	// 45 bottom - 56 top
	draw(scale(1e6, 1e6), RootGetObject(top_dir+"/"+datasets[dsi]+"/distributions_45b_56t.root", "acceptance correction/h_th_y_vs_th_x_after"), "def");
	
	// 45 top - 56 bottom
	draw(scale(1e6, 1e6), RootGetObject(top_dir+"/"+datasets[dsi]+"/distributions_45t_56b.root", "acceptance correction/h_th_y_vs_th_x_after"), "p");
	
	draw((-350, cut_th_y_low_top)--(+350, cut_th_y_low_top), magenta+1pt);
	draw((-350, cut_th_y_low_bot)--(+350, cut_th_y_low_bot), magenta+1pt);
	
	draw((-350, cut_th_y_high_top)--(+350, cut_th_y_high_top), red+1pt);
	draw((-350, cut_th_y_high_bot)--(+350, cut_th_y_high_bot), red+1pt);

	/*
	draw((cut_th_x_low_top , 0)--(cut_th_x_low_top , +200), magenta+1pt);
	draw((cut_th_x_low_bot , -200)--(cut_th_x_low_bot , 0), magenta+1pt);
	draw((cut_th_x_high_top, 0)--(cut_th_x_high_top, +200), magenta+1pt);
	draw((cut_th_x_high_bot, -200)--(cut_th_x_high_bot, 0), magenta+1pt);
	*/
	
	DrawAcceptedArcs(50);
	DrawAcceptedArcs(100);
	DrawAcceptedArcs(150);

	limits((-150, -150), (150, 150), Crop);
	AttachLegend(datasets[dsi]);
}

GShipout(margin=1mm, hSkip=1mm);
