import root;
import pad_layout;

string topDir = "../../";

xSizeDef = 10cm;
ySizeDef = 8cm;

drawGridDef = true;

//----------------------------------------------------------------------------------------------------

string binnings[] = {
	//"ub",
	"ob-1-30-0.05",
};

//----------------------------------------------------------------------------------------------------

NewPad("$|t|\ung{GeV^2}$", "$t$ bin size$\ung{GeV^2}$");
currentpad.xTicks = LeftTicks(0.3, 0.1);
currentpad.yTicks = RightTicks(0.05, 0.01);

AddToLegend("<{\it resolution considerations:}");
draw(scale(1, 1), RootGetObject(topDir+"binning/generators.root", "g_rms_t"), red+longdashed, "1 smearing sigma");
draw(scale(1, 2), RootGetObject(topDir+"binning/generators.root", "g_rms_t"), blue+longdashed, "2 smearing sigma");
draw(scale(1, 3), RootGetObject(topDir+"binning/generators.root", "g_rms_t"), heavygreen+longdashed, "3 smearing sigma");

AddToLegend("<{\it fixed statistical uncertainty, DS4:}");
draw(RootGetObject(topDir+"binning/generators.root", "g_bs_stat_unc_30"), red+dashed, "$30\un{\%}$");
draw(RootGetObject(topDir+"binning/generators.root", "g_bs_stat_unc_20"), blue+dashed, "$20\un{\%}$");
draw(RootGetObject(topDir+"binning/generators.root", "g_bs_stat_unc_10"), heavygreen+dashed, "$10\un{\%}$");

AddToLegend("<{\it binnings in analysis:}");
for (int bi : binnings.keys)
{
	pen p = StdPen(bi);
	draw(RootGetObject(topDir+"DS-fill5313/distributions_45b_56t.root", "binning/g_binning_"+binnings[bi]), "p,l,d0",
		p+0.2pt, mCi+1pt+p, binnings[bi]);
}

limits((0, 0), (1, 0.06), Crop);

frame f_legend = BuildLegend();

NewPad(false);
add(shift(0, 100)*f_legend);
