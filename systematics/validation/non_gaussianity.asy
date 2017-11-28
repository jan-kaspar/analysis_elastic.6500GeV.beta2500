import root;
import pad_layout;

string file1 = "../data-mc/1E7/45b_56t/fitN-2/dx-non-gauss.root";
string obj1 = "h_d_x";

string file2 = "../../non-gaussianity/fit_dx.root";
string obj2 = "ff";

TF1_x_min = -70e-6;
TF1_x_max = +70e-6;

NewPad();
scale(Linear, Log);
draw(RootGetObject(file1, obj1), "eb", blue);
draw(scale(0.869865, 1)*shift(6.357218e-07, -4.26), RootGetObject(file2, obj2), "def", red);
