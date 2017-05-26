import root;
import pad_layout;

string topDir = "../../";

TF1_x_min = -50e-6;
TF1_x_max = +50e-6;

//----------------------------------------------------------------------------------------------------

NewPad("$\th^{*R}_x - \th^{*L}_x\ung{\mu rad}$");
draw(scale(1e6, 1), RootGetObject(topDir+"non-gaussianity/fit_dx.root", "h_in"), "eb", blue);
draw(scale(1e6, 1), RootGetObject(topDir+"non-gaussianity/fit_dx.root", "ff"), red+1pt);
xlimits(-60, +60, Crop);

NewPad("$\th^{*R}_x - \th^{*L}_x\ung{\mu rad}$");
scale(Linear, Log);
draw(scale(1e6, 1), RootGetObject(topDir+"non-gaussianity/fit_dx.root", "h_in"), "eb", blue);
draw(scale(1e6, 1), RootGetObject(topDir+"non-gaussianity/fit_dx.root", "ff"), red+1pt);
xlimits(-60, +60, Crop);

//----------------------------------------------------------------------------------------------------

GShipout("non_gaussianity_fit_dx");
