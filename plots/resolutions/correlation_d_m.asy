import root;
import pad_layout;

string topDir = "../../";

string f = topDir + "/acceptance/mc_resolution_details.root";

NewPad("$m_x\ung{\mu rad}$", "$d_x\ung{\mu rad}$");
draw(scale(1e6, 1e6), RootGetObject(f, "h_th_x_d_vs_th_x_m_acc"));

NewPad("$m_y\ung{\mu rad}$", "$d_y\ung{\mu rad}$");
draw(scale(1e6, 1e6), RootGetObject(f, "h_th_y_d_vs_th_y_m_acc"));
