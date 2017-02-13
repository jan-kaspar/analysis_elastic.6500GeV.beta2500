import root;
import pad_layout;

NewPad("$|t|\ung{GeV^2}$", "");
scale(Linear, Log);
draw(RootGetObject("mc.root", "h_t_sm"), "eb", blue);
draw(RootGetObject("mc.root", "h_t_sm_cut_corr_old"), "eb", heavygreen);
draw(RootGetObject("mc.root", "h_t_sm_cut_corr_new"), "eb", red);
xlimits(0, 1, Crop);

NewPad("$|t|\ung{GeV^2}$", "");
//scale(Linear, Log);
draw(RootGetObject("mc.root", "h_t_lowt_sm"), "eb", blue);
draw(RootGetObject("mc.root", "h_t_lowt_sm_cut_corr_old"), "eb", heavygreen);
draw(RootGetObject("mc.root", "h_t_lowt_sm_cut_corr_new"), "eb", red);
xlimits(0, 0.01, Crop);
