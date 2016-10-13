#include "common_definitions.h"
#include "common_algorithms.h"
#include "parameters.h"
#include "common.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TChain.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TMath.h"
#include "TF1.h"
#include "TTree.h"

#include <cmath>

using namespace std;


//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if (argc < 2)
		return 1;

	// init diagonal settings
	Init(argv[1]);
	if (diagonal != ad45b_56b && diagonal != ad45t_56t)
		return rcIncompatibleDiagonal;

	// default parameters
	unsigned int detailsLevel = 0; 	// 0: no details, 1: some details, >= 2 all details
	bool overrideCutSelection = false;	// whether the default cut selection should be overriden by the command-line selection
	string cutSelectionString;
	string outputDir = ".";
	string inputDir = ".";
	double input_n_si = 5.0;

	// parse command line arguments, starting from index 2
	for (int i = 2; i < argc; i++)
	{
		//printf("%u => %s\n", i, argv[i]);

		if (strcmp(argv[i], "-no-details") == 0)
		{
			detailsLevel = 0;
			continue;
		}

		if (strcmp(argv[i], "-details") == 0)
		{
			if (argc-1 > i)
				detailsLevel = atoi(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-cuts") == 0)
		{
			if (argc-1 > i)
			{
				cutSelectionString = argv[++i];
				overrideCutSelection = true;
			}
			continue;
		}
		
		if (strcmp(argv[i], "-output-dir") == 0)
		{
			if (argc-1 > i)
				outputDir = argv[++i];
			continue;
		}

		if (strcmp(argv[i], "-input-dir") == 0)
		{
			if (argc-1 > i)
				inputDir = argv[++i];
			continue;
		}

		if (strcmp(argv[i], "-n-si") == 0)
		{
			if (argc-1 > i)
				input_n_si = atof(argv[++i]);
			continue;
		}

		printf("ERROR: unknown parameter `%s'.\n", argv[i]);
		return 3;
	}
	
	printf("* detailsLevel = %u\n", detailsLevel);
	printf("* outputDir = %s\n", outputDir.c_str());
	printf("* inputDir = %s\n", inputDir.c_str());
	printf("* input n_si = %.3f\n", input_n_si);
	
	// select cuts
	anal.BuildCuts(); 
	anal.n_si = input_n_si;

	if (overrideCutSelection)
	{
		anal.cuts.clear();
		char buf[100];
		strcpy(buf, cutSelectionString.c_str());

		// workaround for a Bash bug? --> keeping quotes around the parameter
		for (unsigned int i = 0; i < strlen(buf); i++)
			if (buf[i] == '"')
				buf[i] = ' ';

		printf("* setting cuts from selection string [%s]\n", buf);

		if (strcmp(buf, "  ") == 0 || strlen(buf) == 0)
			printf("* cut selection list empty\n");
		else {
			char *pch = strtok(buf, ",");
			while (pch != NULL)
			{
				unsigned int cut = atoi(pch);
				if (cut < 1 || cut > 7)
				{
					printf("ERROR: invalid cut number %u.\n", cut);
					return 2;
				}
				anal.cuts.push_back(cut);
				pch = strtok (NULL, ",");
			}
		}
	}

	// print info
	printf("\n");
	printf("------------------------------ environment ------------------------------\n");
	env.Print();
	printf("\n");
	printf("------------------------------- analysis --------------------------------\n");
	anal.Print();
	printf("\n");

	// alignment init
	for (unsigned int i = 0; i < alignmentSources.size(); ++i)
	{
		printf("\n---------- alignment source %u ----------\n", i);
		alignmentSources[i].Init();
	}
	printf("\n\n");
	
	// binnings
	vector<string> binnings;
	binnings.push_back("ub");
	//binnings.push_back("eb");
	binnings.push_back("ob-1-10-0.2");
	binnings.push_back("ob-1-30-0.2");

	// get input
	TChain *ch_in = new TChain("distilled");
	printf(">> input chain\n");
	for (const auto &ntupleDir : distilledNtuples)
	{
		string f = storageDir + "/" + ntupleDir + "/distill_" + argv[1] + ".root";
		printf("+ %s\n", f.c_str());
		ch_in->Add(f.c_str());
	}
	printf("%llu entries\n", ch_in->GetEntries());

	EventRed ev;
	ch_in->SetBranchAddress("v_L_1_F", &ev.h.L_1_F.v); ch_in->SetBranchAddress("x_L_1_F", &ev.h.L_1_F.x); ch_in->SetBranchAddress("y_L_1_F", &ev.h.L_1_F.y);
	ch_in->SetBranchAddress("v_L_2_N", &ev.h.L_2_N.v); ch_in->SetBranchAddress("x_L_2_N", &ev.h.L_2_N.x); ch_in->SetBranchAddress("y_L_2_N", &ev.h.L_2_N.y);
	ch_in->SetBranchAddress("v_L_2_F", &ev.h.L_2_F.v); ch_in->SetBranchAddress("x_L_2_F", &ev.h.L_2_F.x); ch_in->SetBranchAddress("y_L_2_F", &ev.h.L_2_F.y);

	ch_in->SetBranchAddress("v_R_1_F", &ev.h.R_1_F.v); ch_in->SetBranchAddress("x_R_1_F", &ev.h.R_1_F.x); ch_in->SetBranchAddress("y_R_1_F", &ev.h.R_1_F.y);
	ch_in->SetBranchAddress("v_R_2_N", &ev.h.R_2_N.v); ch_in->SetBranchAddress("x_R_2_N", &ev.h.R_2_N.x); ch_in->SetBranchAddress("y_R_2_N", &ev.h.R_2_N.y);
	ch_in->SetBranchAddress("v_R_2_F", &ev.h.R_2_F.v); ch_in->SetBranchAddress("x_R_2_F", &ev.h.R_2_F.x); ch_in->SetBranchAddress("y_R_2_F", &ev.h.R_2_F.y);	

	ch_in->SetBranchAddress("timestamp", &ev.timestamp);
	ch_in->SetBranchAddress("run_num", &ev.run_num);
	ch_in->SetBranchAddress("bunch_num", &ev.bunch_num);
	ch_in->SetBranchAddress("event_num", &ev.event_num);
	ch_in->SetBranchAddress("trigger_num", &ev.trigger_num);
	ch_in->SetBranchAddress("trigger_bits", &ev.trigger_bits);

	// init output files
	TFile *outF = new TFile((outputDir+"/distributions_anti_" + argv[1] + ".root").c_str(), "recreate");

	// book metadata histograms
	TH1D *h_timestamp_dgn = new TH1D("h_timestamp_dgn", ";timestamp;rate   (Hz)", 97001, 16E3-0.5, 113E3+0.5);
	TH1D *h_timestamp_B0 = new TH1D("h_timestamp_B0", ";timestamp;rate   (Hz)", 97001, 16E3-0.5, 113E3+0.5);
	TH1D *h_timestamp_sel = new TH1D("h_timestamp_sel", ";timestamp;rate   (Hz)", 97001, 16E3-0.5, 113E3+0.5);

	TGraph *g_run_vs_timestamp = new TGraph(); g_run_vs_timestamp->SetName("g_run_vs_timestamp"); g_run_vs_timestamp->SetTitle(";timestamp;run");
	TGraph *g_ev_num_vs_timestamp = new TGraph(); g_ev_num_vs_timestamp->SetName("g_ev_num_vs_timestamp"); g_ev_num_vs_timestamp->SetTitle(";timestamp;ev_num");
	TGraph *g_tr_num_vs_timestamp = new TGraph(); g_tr_num_vs_timestamp->SetName("g_tr_num_vs_timestamp"); g_tr_num_vs_timestamp->SetTitle(";timestamp;tr_num");
	TGraph *g_bunch_num_vs_timestamp = new TGraph(); g_bunch_num_vs_timestamp->SetName("g_bunch_num_vs_timestamp"); g_bunch_num_vs_timestamp->SetTitle(";timestamp;bunch");

	// book hit-distribution histograms
	TH2D *h_y_L_1_F_vs_x_L_1_F_al_nosel = new TH2D("h_y_L_1_F_vs_x_L_1_F_al_nosel", ";x^{L,1,F};y^{L,1,F}", 150, -15., 15., 300, -30., +30.);
	TH2D *h_y_L_2_N_vs_x_L_2_N_al_nosel = new TH2D("h_y_L_2_N_vs_x_L_2_N_al_nosel", ";x^{L,2,N};y^{L,2,N}", 150, -15., 15., 300, -30., +30.);
	TH2D *h_y_L_2_F_vs_x_L_2_F_al_nosel = new TH2D("h_y_L_2_F_vs_x_L_2_F_al_nosel", ";x^{L,2,F};y^{L,2,F}", 150, -15., 15., 300, -30., +30.);
	TH2D *h_y_R_1_F_vs_x_R_1_F_al_nosel = new TH2D("h_y_R_1_F_vs_x_R_1_F_al_nosel", ";x^{R,1,F};y^{R,1,F}", 150, -15., 15., 300, -30., +30.);
	TH2D *h_y_R_2_N_vs_x_R_2_N_al_nosel = new TH2D("h_y_R_2_N_vs_x_R_2_N_al_nosel", ";x^{R,2,N};y^{R,2,N}", 150, -15., 15., 300, -30., +30.);
	TH2D *h_y_R_2_F_vs_x_R_2_F_al_nosel = new TH2D("h_y_R_2_F_vs_x_R_2_F_al_nosel", ";x^{R,2,F};y^{R,2,F}", 150, -15., 15., 300, -30., +30.);
	
	TH2D *h_y_L_1_F_vs_x_L_1_F_al_sel = new TH2D("h_y_L_1_F_vs_x_L_1_F_al_sel", ";x^{L,1,F};y^{L,1,F}", 100, -3., +3., 300, -30., +30.);
	TH2D *h_y_L_2_N_vs_x_L_2_N_al_sel = new TH2D("h_y_L_2_N_vs_x_L_2_N_al_sel", ";x^{L,2,N};y^{L,2,N}", 100, -3., +3., 300, -30., +30.);
	TH2D *h_y_L_2_F_vs_x_L_2_F_al_sel = new TH2D("h_y_L_2_F_vs_x_L_2_F_al_sel", ";x^{L,2,F};y^{L,2,F}", 100, -3., +3., 300, -30., +30.);
	TH2D *h_y_R_1_F_vs_x_R_1_F_al_sel = new TH2D("h_y_R_1_F_vs_x_R_1_F_al_sel", ";x^{R,1,F};y^{R,1,F}", 100, -3., +3., 300, -30., +30.);
	TH2D *h_y_R_2_N_vs_x_R_2_N_al_sel = new TH2D("h_y_R_2_N_vs_x_R_2_N_al_sel", ";x^{R,2,N};y^{R,2,N}", 100, -3., +3., 300, -30., +30.);
	TH2D *h_y_R_2_F_vs_x_R_2_F_al_sel = new TH2D("h_y_R_2_F_vs_x_R_2_F_al_sel", ";x^{R,2,F};y^{R,2,F}", 100, -3., +3., 300, -30., +30.);

	// book cut histograms
	map<unsigned int, TH1D *> h_cq;
	map<unsigned int, TH2D *> h2_cq, h2_cq_full;
	//map<unsigned int, TGraph *> g_cq;
	map<unsigned int, TProfile *> p_cq, p_cq_time;
	for (unsigned int i = 1; i <= anal.N_cuts; ++i)
	{
		char name[100], title[100];

		double x_min=0., x_max=0., y_min=0., y_max = 0.;
		double q_max = 0.;

		if (i == 1) { x_min = -200E-6; x_max = +200E-6; y_min = -200E-6; y_max = 200E-6; q_max = 50E-6; }
		if (i == 2) { x_min = -150E-6; x_max = +150E-6; y_min = -150E-6; y_max = 150E-6; q_max = 15E-6; }
		if (i == 3) { x_min = -1000E-6; x_max = +1000E-6; y_min = -1.5; y_max = 1.5; }
		if (i == 4) { x_min = -1000E-6; x_max = +1000E-6; y_min = -1.5; y_max = 1.5; }
		if (i == 5) { x_min = -30.; x_max = +30.; y_min = -3.; y_max = 3.; q_max = 150E-3; }
		if (i == 6) { x_min = -30.; x_max = +30.; y_min = -3.; y_max = 3.; q_max = 150E-3; }
		if (i == 7) { x_min = -200E-6; x_max = +200E-6; y_min = -0.2; y_max = +0.2; q_max = 100E-3; }
		if (i == 8) { x_min = -600E-6; x_max = +600E-6; y_min = -4.; y_max = +4.; q_max = 500E-3; }
		
		sprintf(name, "h_cq%i", i); sprintf(title, ";cq%i", i); h_cq[i] = new TH1D(name, title, 300, -q_max, +q_max);

		sprintf(name, "h2_cq%i", i); sprintf(title, ";%s;%s", anal.cqaN[i].c_str(), anal.cqbN[i].c_str()); h2_cq[i] = new TH2D(name, title, 100, x_min, x_max, 100, y_min, y_max);
		sprintf(name, "h2_cq_full%i", i); sprintf(title, ";%s;%s", anal.cqaN[i].c_str(), anal.cqbN[i].c_str()); h2_cq_full[i] = new TH2D(name, title, 100, x_min, x_max, 100, y_min, y_max);

		//sprintf(name, "g_cq%i", i); sprintf(title, ";%s;%s", anal.cqaN[i].c_str(), anal.cqbN[i].c_str()); g_cq[i] = new TGraph(); g_cq[i]->SetName(name); g_cq[i]->SetTitle(title);
		sprintf(name, "p_cq%i", i); sprintf(title, ";%s;%s", anal.cqaN[i].c_str(), anal.cqbN[i].c_str()); p_cq[i] = new TProfile(name, title, 300, 0., 0.);
		sprintf(name, "p_cq_time%i", i); sprintf(title, ";time   (s);mean of cq%i", i); p_cq_time[i] = new TProfile(name, title, 240, 6E3-0.5, 30E3+0.5);
	}

	// book angluar histograms
#if 0
	TH1D *th_x_diffLR = new TH1D("th_x_diffLR", ";#theta_{x}^{R} - #theta_{x}^{L}", 1000, -500E-6, +500E-6); th_x_diffLR->Sumw2();
	TH1D *th_y_diffLR = new TH1D("th_y_diffLR", ";#theta_{y}^{R} - #theta_{y}^{L}", 1000, -50E-6, +50E-6); th_y_diffLR->Sumw2();

	TH1D *th_x_diffLF = new TH1D("th_x_diffLF", ";#theta_{x}^{L} - #theta_{x}", 400, -200E-6, +200E-6); th_x_diffLF->Sumw2();
	TH1D *th_x_diffRF = new TH1D("th_x_diffRF", ";#theta_{x}^{R} - #theta_{x}", 400, -200E-6, +200E-6); th_x_diffRF->Sumw2();
	
	TH2D *h_th_x_diffLR_vs_th_x = new TH2D("h_th_x_diffLR_vs_th_x", ";#theta_{x};#theta_{x}^{R} - #theta_{x}^{L}", 100, -200E-6, +200E-6, 100, -50E-6, +50E-6);
	TH2D *h_th_y_diffLR_vs_th_y = new TH2D("h_th_y_diffLR_vs_th_y", ";#theta_{y};#theta_{y}^{R} - #theta_{y}^{L}", 120, -120E-6, +120E-6, 150, -15E-6, +15E-6);
	
	TProfile *p_th_x_diffLR_vs_th_x = new TProfile("p_th_x_diffLR_vs_th_x", ";#theta_{x};#theta_{x}^{R} - #theta_{x}^{L}", 200, -200E-6, +200E-6);
	TProfile *p_th_y_diffLR_vs_th_y = new TProfile("p_th_y_diffLR_vs_th_y", ";#theta_{y};#theta_{y}^{R} - #theta_{y}^{L}", 120, -120E-6, +120E-6);
	TProfile *p_th_y_L_diffNF_vs_th_y_L = new TProfile("p_th_y_L_diffNF_vs_th_y_L", ";#theta_{y}^{L};#theta_{y}^{LF} - #theta_{y}^{LN}", 120, -120E-6, +120E-6);
	TProfile *p_th_y_R_diffNF_vs_th_y_R = new TProfile("p_th_y_R_diffNF_vs_th_y_R", ";#theta_{y}^{R};#theta_{y}^{RF} - #theta_{y}^{RN}", 120, -120E-6, +120E-6);
	
	TH1D *th_x_diffLR_safe = new TH1D("th_x_diffLR_safe", ";#theta_{x}^{R} - #theta_{x}^{L}", 1000, -500E-6, +500E-6); th_x_diffLR_safe->Sumw2();
	TH1D *th_y_diffLR_safe = new TH1D("th_y_diffLR_safe", ";#theta_{y}^{R} - #theta_{y}^{L}", 1000, -50E-6, +50E-6); th_y_diffLR_safe->Sumw2();

	TProfile *p_th_x_L_vs_th_y_L = new TProfile("p_th_x_L_vs_th_y_L", ";#theta_{y}^{L};#theta_{x}^{L}", 30, 0., 0.);
	TProfile *p_th_x_R_vs_th_y_R = new TProfile("p_th_x_R_vs_th_y_R", ";#theta_{y}^{R};#theta_{x}^{R}", 30, 0., 0.);

	TH2D *h_th_y_L_vs_th_x_L = new TH2D("h_th_y_L_vs_th_x_L", ";#theta_{x}^{L};#theta_{y}^{L}", 150, -150E-6, +150E-6, 750, -150E-6, +150E-6);
	TH2D *h_th_y_R_vs_th_x_R = new TH2D("h_th_y_R_vs_th_x_R", ";#theta_{x}^{R};#theta_{y}^{R}", 150, -150E-6, +150E-6, 750, -150E-6, +150E-6);
	TH2D *h_th_y_vs_th_x = new TH2D("h_th_y_vs_th_x", ";#theta_{x};#theta_{y}", 100, -150E-6, +150E-6, 100, -150E-6, +150E-6);
	
	TGraph *g_th_y_L_vs_th_x_L = new TGraph(); g_th_y_L_vs_th_x_L->SetName("g_th_y_L_vs_th_x_L"); g_th_y_L_vs_th_x_L->SetTitle(";#theta_{x}^{L};#theta_{y}^{L}");
	TGraph *g_th_y_R_vs_th_x_R = new TGraph(); g_th_y_R_vs_th_x_R->SetName("g_th_y_R_vs_th_x_R"); g_th_y_R_vs_th_x_R->SetTitle(";#theta_{x}^{R};#theta_{y}^{R}");
	TGraph *g_th_y_vs_th_x = new TGraph(); g_th_y_vs_th_x->SetName("g_th_y_vs_th_x"); g_th_y_vs_th_x->SetTitle(";#theta_{x}^{L};#theta_{y}^{L}");
	
	TH2D *h_th_y_L_vs_th_y_R = new TH2D("h_th_y_L_vs_th_y_R", ";#theta_{y}^{R};#theta_{y}^{L}", 300, -150E-6, +150E-6, 300, -150E-6, +150E-6);
	TGraph *g_th_y_L_vs_th_y_R = new TGraph(); g_th_y_L_vs_th_y_R->SetName("g_th_y_L_vs_th_y_R"); g_th_y_L_vs_th_y_R->SetTitle(";#theta_{y}^{R};#theta_{y}^{L}");
	
	TH1D *h_th_x_raw = new TH1D("h_th_x_raw", ";#theta_{x}", 200, -200E-6, +200E-6);
	TH1D *h_th_y_raw = new TH1D("h_th_y_raw", ";#theta_{y}", 960, -120E-6, +120E-6);
	TH1D *h_th_y_raw_flipped = new TH1D("h_th_y_raw_flipped", ";#theta_{y}", 960, -120E-6, +120E-6);
	
	TH1D *h_th_x_L = new TH1D("h_th_x_L", ";#theta_{x}^{L}", 200, -200E-6, +200E-6); h_th_x_L->SetLineColor(4);
	TH1D *h_th_x_R = new TH1D("h_th_x_R", ";#theta_{x}^{R}", 200, -200E-6, +200E-6); h_th_x_R->SetLineColor(2);
	
	TH1D *h_th_y_L = new TH1D("h_th_y_L", ";#theta_{y}^{L}", 960, -120E-6, +120E-6); h_th_y_L->SetLineColor(4);
	TH1D *h_th_y_R = new TH1D("h_th_y_R", ";#theta_{y}^{R}", 960, -120E-6, +120E-6); h_th_y_R->SetLineColor(2);
#endif
	
	// book acceptance-correction histograms
	TProfile *p_t_ub_div_corr = new TProfile("p_t_ub_div_corr", ";t_ub_{y}", 2000., 0., 0.2);

	map<unsigned int, TH1D*> bh_t_Nev_before, bh_t_Nev_after_no_corr;
	map<unsigned int, TH1D*> bh_t_before, bh_t_after_no_corr, bh_t_after;
	map<unsigned int, TProfile*> bp_t_phi_corr, bp_t_full_corr;

	for (unsigned int bi = 0; bi < binnings.size(); ++bi)
	{
		unsigned int N_bins;
		double *bin_edges;
		BuildBinning(anal, binnings[bi], bin_edges, N_bins);

		bh_t_Nev_before[bi] = new TH1D("h_t_Nev_before", ";|t|;events per bin", N_bins, bin_edges); bh_t_Nev_before[bi]->Sumw2();
		bh_t_Nev_after_no_corr[bi] = new TH1D("h_t_Nev_after_no_corr", ";|t|;events per bin", N_bins, bin_edges); bh_t_Nev_after_no_corr[bi]->Sumw2();
		bh_t_before[bi] = new TH1D("h_t_before", ";|t|", N_bins, bin_edges); bh_t_before[bi]->Sumw2();
		bh_t_after[bi] = new TH1D("h_t_after", ";|t|", N_bins, bin_edges); bh_t_after[bi]->Sumw2();
		bh_t_after_no_corr[bi] = new TH1D("h_t_after_no_corr", ";|t|", N_bins, bin_edges); bh_t_after_no_corr[bi]->Sumw2();
		bp_t_phi_corr[bi] = new TProfile("p_t_phi_corr", ";t", N_bins, bin_edges, "s");
		bp_t_full_corr[bi] = new TProfile("p_t_full_corr", ";t", N_bins, bin_edges, "s");
	}

	TH2D *h_th_y_vs_th_x_before = new TH2D("h_th_y_vs_th_x_before", ";#theta_{x};#theta_{y}", 150, -150E-6, +150E-6, 150, -150E-6, +150E-6); h_th_y_vs_th_x_before->Sumw2();
	TH2D *h_th_y_vs_th_x_after = new TH2D("h_th_y_vs_th_x_after", ";#theta_{x};#theta_{y}", 150, -150E-6, +150E-6, 150, -150E-6, +150E-6); h_th_y_vs_th_x_after->Sumw2();
	TH2D *h_th_vs_phi_after = new TH2D("h_th_vs_phi_after", ";#phi;#theta", 100, 0, 0, 100, 0, 0); h_th_vs_phi_after->Sumw2();

	// event loop
	for (int ev_idx = 0; ev_idx < ch_in->GetEntries(); ++ev_idx)
	{
		ch_in->GetEntry(ev_idx);

		// remove troublesome runs
		unsigned int run = ev.run_num / 100000;
		unsigned int file = ev.run_num % 100000;
		if (SkipRun(run, file, true))
			continue;

		// check time - selected?
		if (anal.SkipTime(ev.timestamp))
			continue;

		// diagonal cut
		bool allDiagonalRPs = (ev.h.L_2_N.v && ev.h.L_2_F.v && ev.h.R_2_N.v && ev.h.R_2_F.v);
		if (!allDiagonalRPs)
			continue;
		
		h_timestamp_dgn->Fill(ev.timestamp);

		// select the elastic-trigger bunch(es) only
		if (SkipBunch(run, ev.bunch_num))
			continue;

		// zero bias event?
		//bool zero_bias_event = IsZeroBias(ev.trigger_bits, ev.run_num, ev.event_num);

		// apply fine alignment
		HitData h_al = ev.h;
		for (unsigned int i = 0; i < alignmentSources.size(); ++i)
		{
			AlignmentData alData = alignmentSources[i].Eval(ev.timestamp);
			h_al = h_al.ApplyAlignment(alData);
		}

		// fill pre-selection histograms
		h_y_L_1_F_vs_x_L_1_F_al_nosel->Fill(h_al.L_1_F.x, h_al.L_1_F.y);
		h_y_L_2_N_vs_x_L_2_N_al_nosel->Fill(h_al.L_2_N.x, h_al.L_2_N.y);
		h_y_L_2_F_vs_x_L_2_F_al_nosel->Fill(h_al.L_2_F.x, h_al.L_2_F.y);
		h_y_R_1_F_vs_x_R_1_F_al_nosel->Fill(h_al.R_1_F.x, h_al.R_1_F.y);
		h_y_R_2_N_vs_x_R_2_N_al_nosel->Fill(h_al.R_2_N.x, h_al.R_2_N.y);
		h_y_R_2_F_vs_x_R_2_F_al_nosel->Fill(h_al.R_2_F.x, h_al.R_2_F.y);

		if (detailsLevel >= 2)
		{
			h_timestamp_B0->Fill(ev.timestamp);
			g_run_vs_timestamp->SetPoint(g_run_vs_timestamp->GetN(), ev.timestamp, ev.run_num);
			//g_ev_num_vs_timestamp->SetPoint(g_ev_num_vs_timestamp->GetN(), ev.timestamp, ev.event_num);
			//g_tr_num_vs_timestamp->SetPoint(g_tr_num_vs_timestamp->GetN(), ev.timestamp, ev.trigger_num);
		}

		// coordinate inversion in sector 45
		//h_al.x_L_N = - h_al.x_L_N;
        //h_al.x_L_F = - h_al.x_L_F;

		h_al.L_1_F.y = - h_al.L_1_F.y;
		h_al.L_2_N.y = - h_al.L_2_N.y;
        h_al.L_2_F.y = - h_al.L_2_F.y;

		// run reconstruction
		Kinematics k = DoReconstruction(h_al, env);

		// cut evaluation
		CutData cd;
		bool select = anal.EvaluateCuts(h_al, k, cd);
		//bool bckg = !select;

		// fill no-cut histograms
		for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
		{
			//h2_cq_full[ci]->Fill(ccb[ci]*cqa[ci] - cca[ci]*cqb[ci], cca[ci]*cqa[ci] + ccb[ci]*cqb[ci] + ccc[ci]);
			h2_cq_full[ci]->Fill(cd.cqa[ci], cd.cqb[ci]);
		}
		
		// apply elastic cut
		if (!select)
			continue;

		// enforce RP trigger (vertical || horizontal)
		/*
		if ((ev.trigger_bits & 3) == 0)
			continue;
		*/

		// histograms
		h_timestamp_sel->Fill(ev.timestamp);
		
		for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
		{
			h_cq[ci]->Fill(cd.cv[ci]);
			h2_cq[ci]->Fill(cd.cqa[ci], cd.cqb[ci]);
			//h2_cq[ci]->Fill(ccb[ci]*cqa[ci] - cca[ci]*cqb[ci], cca[ci]*cqa[ci] + ccb[ci]*cqb[ci] + ccc[ci]);
			/*
			if (detailsLevel >= 2)
				g_cq[ci]->SetPoint(g_cq[ci]->GetN(), cd.cqa[ci], cd.cqb[ci]);
			*/
			p_cq[ci]->Fill(cd.cqa[ci], cd.cqb[ci]);
			p_cq_time[ci]->Fill(ev.timestamp, cd.cv[ci]);
		}

		h_y_L_1_F_vs_x_L_1_F_al_sel->Fill(h_al.L_1_F.x, h_al.L_1_F.y);
		h_y_L_2_N_vs_x_L_2_N_al_sel->Fill(h_al.L_2_N.x, h_al.L_2_N.y);
		h_y_L_2_F_vs_x_L_2_F_al_sel->Fill(h_al.L_2_F.x, h_al.L_2_F.y);
		h_y_R_1_F_vs_x_R_1_F_al_sel->Fill(h_al.R_1_F.x, h_al.R_1_F.y);
		h_y_R_2_N_vs_x_R_2_N_al_sel->Fill(h_al.R_2_N.x, h_al.R_2_N.y);
		h_y_R_2_F_vs_x_R_2_F_al_sel->Fill(h_al.R_2_F.x, h_al.R_2_F.y);

		/*
		th_x_diffLR->Fill(k.th_x_R - k.th_x_L);
		th_y_diffLR->Fill(k.th_y_R - k.th_y_L);

		th_x_diffLF->Fill(k.th_x_L - k.th_x);
		th_x_diffRF->Fill(k.th_x_R - k.th_x);

		h_th_x_diffLR_vs_th_x->Fill(k.th_x, k.th_x_R - k.th_x_L);
		h_th_y_diffLR_vs_th_y->Fill(k.th_y, k.th_y_R - k.th_y_L);

		p_th_x_diffLR_vs_th_x->Fill(k.th_x, k.th_x_R - k.th_x_L);
		p_th_y_diffLR_vs_th_y->Fill(k.th_y, k.th_y_R - k.th_y_L);
		p_th_y_L_diffNF_vs_th_y_L->Fill(k.th_y_L, k.th_y_L_F - k.th_y_L_N);
		p_th_y_R_diffNF_vs_th_y_R->Fill(k.th_y_R, k.th_y_R_F - k.th_y_R_N);
		
		double safe_th_y_min = anal.th_y_lcut + 5E-6;
		double safe_th_y_max = anal.th_y_hcut - 5E-6;
		if (fabs(k.th_y) > safe_th_y_min && fabs(k.th_y) < safe_th_y_max)
		{
			th_y_diffLR_safe->Fill(k.th_y_R - k.th_y_L);
			th_x_diffLR_safe->Fill(k.th_x_R - k.th_x_L);
		}
	
		p_th_x_L_vs_th_y_L->Fill(k.th_y_L, k.th_x_L);
		p_th_x_R_vs_th_y_R->Fill(k.th_y_R, k.th_x_R);
	
		h_th_y_L_vs_th_x_L->Fill(k.th_x_L, k.th_y_L);
		h_th_y_R_vs_th_x_R->Fill(k.th_x_R, k.th_y_R);
		h_th_y_vs_th_x->Fill(k.th_x, k.th_y);
	
		if (detailsLevel >= 1)
		{
			g_th_y_L_vs_th_x_L->SetPoint(g_th_y_L_vs_th_x_L->GetN(), k.th_x_L, k.th_y_L);
			g_th_y_R_vs_th_x_R->SetPoint(g_th_y_R_vs_th_x_R->GetN(), k.th_x_R, k.th_y_R);
		}
	
		h_th_y_L_vs_th_y_R->Fill(k.th_y_R, k.th_y_L);
		if (detailsLevel >= 2)
			g_th_y_L_vs_th_y_R->SetPoint(g_th_y_L_vs_th_y_R->GetN(), k.th_y_R, k.th_y_L);

		h_th_x_raw->Fill(k.th_x);
		h_th_y_raw->Fill(k.th_y);
		h_th_y_raw_flipped->Fill(-k.th_y);
		
		h_th_x_L->Fill(k.th_x_L);
		h_th_x_R->Fill(k.th_x_R);
		
		h_th_y_L->Fill(k.th_y_L);
		h_th_y_R->Fill(k.th_y_R);
		*/

		// calculate acceptance divergence correction
		double phi_corr = 0., div_corr = 0.;
		
		bool skip = CalculateAcceptanceCorrections(th_y_sign, k, anal, phi_corr, div_corr);

		for (unsigned int bi = 0; bi < binnings.size(); bi++)
		{
			bh_t_Nev_before[bi]->Fill(k.t, 1.);
			bh_t_before[bi]->Fill(k.t, 1.);
		}

		h_th_y_vs_th_x_before->Fill(k.th_x, k.th_y, 1.);

		if (skip)
			continue;

		double corr = div_corr * phi_corr;

		// fill acceptance histograms
		p_t_ub_div_corr->Fill(k.t_y, div_corr);

		for (unsigned int bi = 0; bi < binnings.size(); bi++)
		{
			bh_t_Nev_after_no_corr[bi]->Fill(k.t, 1.);

			bh_t_after_no_corr[bi]->Fill(k.t, 1.);
			bh_t_after[bi]->Fill(k.t, corr);

			bp_t_phi_corr[bi]->Fill(k.t, phi_corr);
			bp_t_full_corr[bi]->Fill(k.t, corr);
		}
		
		h_th_y_vs_th_x_after->Fill(k.th_x, k.th_y, div_corr);
		h_th_vs_phi_after->Fill(k.phi, k.th, div_corr);
	}
	
	printf("---------------------------- after event loop ---------------------------\n");

	// normalize histograms
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		bh_t_before[bi]->Scale(1., "width");
		bh_t_after_no_corr[bi]->Scale(1., "width");
		bh_t_after[bi]->Scale(1., "width");
	
		//bh_t_normalized[bi]->Scale(1., "width");
	}
	
/*
	h_th_y_vs_th_x_normalized->Scale(1., "width");
	
	th_y_diffLR->Scale(1., "width");
	th_x_diffLR->Scale(1., "width");
	th_y_diffLR_safe->Scale(1., "width");
	th_x_diffLR_safe->Scale(1., "width");
*/

	// fit histograms
/*
	double th_y_low_bound  = (diagonal == d45b_56t) ? anal.th_y_lcut + 5E-6 : -(anal.th_y_hcut - 5E-6);
	double th_y_high_bound = (diagonal == d45b_56t) ? anal.th_y_hcut - 5E-6 : -(anal.th_y_lcut + 5E-6);
	printf("\n* th_y fit bounds: from %E to %E\n", th_y_low_bound, th_y_high_bound);

	printf("\n* fitting p_th_x_diffLR_vs_th_x\n");
	p_th_x_diffLR_vs_th_x->Fit("pol1", "", "", -100E-6, +100E-6);
	printf("\n* fitting p_th_y_diffLR_vs_th_y\n");
	p_th_y_diffLR_vs_th_y->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);
	printf("\n* fitting p_th_y_L_diffNF_vs_th_y_L\n");
	p_th_y_L_diffNF_vs_th_y_L->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);
	printf("\n* fitting p_th_y_R_diffNF_vs_th_y_R\n");
	p_th_y_R_diffNF_vs_th_y_R->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);
	
	printf("* fitting p_vtx_x_diffLR_vs_th_x\n");
	p_vtx_x_diffLR_vs_th_x->Fit("pol1", "", "", -120E-6, +120E-6);

	printf("* fitting p_th_x_L_vs_th_y_L\n");
	p_th_x_L_vs_th_y_L->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);
	printf("* fitting p_th_x_R_vs_th_y_R\n");
	p_th_x_R_vs_th_y_R->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);

	printf("* fitting h_th_x_R\n");
	h_th_x_R->Fit("gaus", "", "", -50E-6, +50E-6);
	printf("* fitting h_th_x_L\n");
	h_th_x_L->Fit("gaus", "", "", -50E-6, +50E-6);

	printf("* fitting p_x_L_N_vs_th_x_L\n");
	p_x_L_N_vs_th_x_L->Fit("pol1");
	printf("* fitting p_x_L_F_vs_th_x_L\n");
	p_x_L_F_vs_th_x_L->Fit("pol1");
	printf("* fitting p_x_R_N_vs_th_x_R\n");
	p_x_R_N_vs_th_x_R->Fit("pol1");
	printf("* fitting p_x_R_F_vs_th_x_R\n");
	p_x_R_F_vs_th_x_R->Fit("pol1");

	th_y_diffLR_safe->Fit("gaus");
	th_x_diffLR_safe->Fit("gaus");

	printf("* fitting p_ta_th_x_vs_th_x\n");
	p_ta_th_x_vs_th_x->Fit("pol1", "", "", -100E-6, +100E-6);
	
	printf("* fitting p_ta_th_y_vs_th_y\n");
	p_ta_th_y_vs_th_y->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);
*/

	// save histograms
	TCanvas *c;
	
	gDirectory = outF->mkdir("metadata");
	if (detailsLevel >= 2)
	{
		h_timestamp_dgn->Write();
		h_timestamp_B0->SetLineColor(4);
		h_timestamp_B0->Write();
		h_timestamp_sel->SetLineColor(2);
		h_timestamp_sel->Write();

		c = new TCanvas("rate cmp");
		h_timestamp_dgn->Draw();
		h_timestamp_B0->Draw("sames");
		h_timestamp_sel->Draw("sames");
		c->Write();
	
		g_run_vs_timestamp->Write();
		//g_ev_num_vs_timestamp->Write();
		//g_tr_num_vs_timestamp->Write();
		g_bunch_num_vs_timestamp->Write();
	}
	
	TDirectory *hitDistDir = outF->mkdir("hit distributions");
	gDirectory = hitDistDir->mkdir("vertical, aligned, before selection");
	h_y_L_1_F_vs_x_L_1_F_al_nosel->Write();
	h_y_L_2_N_vs_x_L_2_N_al_nosel->Write();
	h_y_L_2_F_vs_x_L_2_F_al_nosel->Write();
	h_y_R_1_F_vs_x_R_1_F_al_nosel->Write();
	h_y_R_2_N_vs_x_R_2_N_al_nosel->Write();
	h_y_R_2_F_vs_x_R_2_F_al_nosel->Write();
	
	gDirectory->Write();
	h_y_L_1_F_vs_x_L_1_F_al_sel->Write();
	h_y_L_2_N_vs_x_L_2_N_al_sel->Write();
	h_y_L_2_F_vs_x_L_2_F_al_sel->Write();
	h_y_R_1_F_vs_x_R_1_F_al_sel->Write();
	h_y_R_2_N_vs_x_R_2_N_al_sel->Write();
	h_y_R_2_F_vs_x_R_2_F_al_sel->Write();

	TDirectory *cutDir = outF->mkdir("elastic cuts");
	for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
	{
		char buf[100];
		sprintf(buf, "cut %u", ci);
		gDirectory = cutDir->mkdir(buf);

		printf("* RMS of cut distribution %u\n", ci);
		printf("%E\n", h_cq[ci]->GetRMS());

		printf("* fitting cut distribution %u\n", ci);
		h_cq[ci]->Fit("gaus", "", "", -3., +3.);

		h_cq[ci]->Write();
		h2_cq[ci]->Write();
		h2_cq_full[ci]->Write();
		//g_cq[ci]->Write();
		
		p_cq[ci]->Write();

		p_cq_time[ci]->Write();

		h2_cq_full[ci]->Draw();

		sprintf(buf, "plot_before_cq%u", ci);
		c = new TCanvas(buf);
		c->SetLogz(1);
		h2_cq_full[ci]->Draw("colz");
		double lim = h2_cq_full[ci]->GetXaxis()->GetXmax();

		double qa[2] = {-lim, +lim};
		double qbp[2]= {(+anal.n_si*anal.csi[ci] - anal.cca[ci]*qa[0] - anal.ccc[ci])/anal.ccb[ci],
			(+anal.n_si*anal.csi[ci] - anal.cca[ci]*qa[1] - anal.ccc[ci])/anal.ccb[ci]};
		double qbm[2]= {(-anal.n_si*anal.csi[ci] - anal.cca[ci]*qa[0] - anal.ccc[ci])/anal.ccb[ci],
			(-anal.n_si*anal.csi[ci] - anal.cca[ci]*qa[1] - anal.ccc[ci])/anal.ccb[ci]};
		TGraph *gP = new TGraph(2, qa, qbp); gP->Draw("l");
		TGraph *gM = new TGraph(2, qa, qbm); gM->Draw("l");
		c->Write();
		
		sprintf(buf, "plot_after_cq%u", ci);
		c = new TCanvas(buf);
		c->SetLogz(1);
		h2_cq[ci]->Draw("colz");
		gP = new TGraph(2, qa, qbp); gP->Draw("l");
		gM = new TGraph(2, qa, qbm); gM->Draw("l");
		gP->Draw("l");
		gM->Draw("l");
		c->Write();
	}

	gDirectory = outF->mkdir("selected - angles");
/*
	th_x_diffLR->Write();
	th_y_diffLR->Write();

	th_x_diffLF->Write();
	th_x_diffRF->Write();
	
	h_th_x_diffLR_vs_th_x->Write();
	h_th_y_diffLR_vs_th_y->Write();

	p_th_x_diffLR_vs_th_x->Write();
	p_th_y_diffLR_vs_th_y->Write();
	p_th_y_L_diffNF_vs_th_y_L->Write();
	p_th_y_R_diffNF_vs_th_y_R->Write();

	th_x_sigmaLR_vs_th_x->Write();
	th_y_sigmaLR_vs_th_y->Write();
	
	th_x_diffLR_safe->Write();
	th_y_diffLR_safe->Write();

	p_th_x_L_vs_th_y_L->Write();
	p_th_x_R_vs_th_y_R->Write();

	h_th_y_L_vs_th_x_L->Write();
	h_th_y_R_vs_th_x_R->Write();
	h_th_y_vs_th_x->Write();

	g_th_y_L_vs_th_x_L->Write();
	g_th_y_R_vs_th_x_R->Write();
	g_th_y_vs_th_x->Write();
	
	h_th_y_L_vs_th_y_R->Write();
	g_th_y_L_vs_th_y_R->Write();
	
	h_th_x_raw->Write();
	h_th_y_raw->Write();
	h_th_y_raw_flipped->Write();
	
	h_th_x_L->Write();
	h_th_x_R->Write();
	
	h_th_y_L->Write();
	h_th_y_R->Write();

	{
		double x[] = {0, 1, 2, 3};
		double y[] = {anal.th_y_lcut_L, anal.th_y_hcut_L, anal.th_y_lcut_R, anal.th_y_hcut_R};
		TGraph *g = new TGraph(4, x, y);
		g->SetName("g_th_y_cuts");
		g->Write();
	}
*/

	gDirectory = outF->mkdir("binning");
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		TGraph *g = new TGraph();
		g->SetName(("g_binning_"+binnings[bi]).c_str());
		g->SetTitle(";bin center;bin width");

		TH1D *h = bh_t_Nev_before[bi];
		for (int bin = 1; bin <= h->GetNbinsX(); bin++)
			g->SetPoint(g->GetN(), h->GetBinCenter(bin), h->GetBinWidth(bin));

		g->Write();
	}

	TDirectory *accDir = outF->mkdir("acceptance correction");
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		gDirectory = accDir->mkdir(binnings[bi].c_str());
		bh_t_Nev_before[bi]->Write();
		bh_t_Nev_after_no_corr[bi]->Write();
		bh_t_before[bi]->Write();
		bh_t_after_no_corr[bi]->Write();
		bh_t_after[bi]->Write();
		bp_t_phi_corr[bi]->Write();
		bp_t_full_corr[bi]->Write();
	
		c = new TCanvas("t cmp");
		c->SetLogy(1);
		bh_t_after[bi]->Draw("");
		bh_t_before[bi]->Draw("same");
		c->Write();
	}
		
	gDirectory = accDir;
	
	p_t_ub_div_corr->Write();
	
	h_th_y_vs_th_x_before->Write();
	h_th_y_vs_th_x_after->Write();
	h_th_vs_phi_after->Write();

	return 0;
}
