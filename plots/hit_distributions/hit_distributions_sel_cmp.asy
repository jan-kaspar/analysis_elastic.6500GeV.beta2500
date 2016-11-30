import root;
import pad_layout;

string topDir = "../../";

TH2_palette = Gradient(blue, heavygreen, yellow, red);

string f = topDir + "DS-fill5313/distributions_45t_56b.root";

string rp = "L_2_F";

//xTicksDef = LeftTicks(Step=10, step=5);
//yTicksDef = RightTicks(Step=10, step=5);
//yTicksDef = RightTicks(Step=0.5, step=0.1);

drawGridDef = true;

//----------------------------------------------------------------------------------------------------

TH2_y_min = 0;
TH2_y_max = 5;

//----------------------------------------------------------------------------------------------------

NewPad("$x\ung{mm}$", "$y\ung{mm}$");
scale(Linear, Linear, Log);

TH2_z_max = 2e3;

draw(RootGetObject(f, "hit distributions/vertical, aligned, before selection/h_y_"+rp+"_vs_x_"+rp+"_al_nosel"), "p,bar");

limits((-10, 0), (+10, +5), Crop);
//AttachLegend("no cuts", S, N);

//----------------------------------------------------------------------------------------------------

NewPad("$x\ung{mm}$", "$y\ung{mm}$");
scale(Linear, Linear, Log);

TH2_z_max = 2e3;

draw(RootGetObject(f, "hit distributions/vertical, aligned, after selection/h_y_"+rp+"_vs_x_"+rp+"_al_sel"), "p,bar");

limits((-10, 0), (+10, +5), Crop);
//AttachLegend("elastic cuts (correlation with 210m RP)", S, N);
