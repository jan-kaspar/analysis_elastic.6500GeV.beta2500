#include "input_files.h"

#include "common_definitions.h"
#include "common_algorithms.h"
#include "parameters.h"
#include "common.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TChain.h"
#include "TH1D.h"

#include "TotemAnalysis/TotemNtuplizer/interface/TriggerDataFormats.h"
#include "TotemAnalysis/TotemNtuplizer/interface/RawDataFormats.h"
#include "TotemAnalysis/TotemNtuplizer/interface/RPRootTrackInfo.h"

#include <csignal>

using namespace std;

//----------------------------------------------------------------------------------------------------

bool interrupt_loop = false;

void SigIntHandler(int)
{
	interrupt_loop = true;
}

//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("ERROR: missing argument (diagonal).\n");
		return 1;
	}

	Init(argv[1]);
	if (diagonal == dUnknown)
	{
		printf("ERROR: unknown diagonal %s\n", argv[1]);
		return 1;
	}

	if (diagonal == dCombined)
		return rcIncompatibleDiagonal;

	InitInputFiles();
	TChain *ch = new TChain("TotemNtuple");
	printf(">> input files\n");
	for (unsigned int i = 0; i < input_files.size(); i++)
	{
		ch->Add(input_files[i].c_str());
		printf("    %s\n", input_files[i].c_str());
	}
	printf(">> chain entries: %llu\n", ch->GetEntries());

	// select and link input branches
	ch->SetBranchStatus("*", 0);

	EventMetaData *metaData = new EventMetaData();
	ch->SetBranchStatus("event_info.*", 1);
	ch->SetBranchAddress("event_info.", &metaData);

	TriggerData *triggerData = new TriggerData();
	ch->SetBranchStatus("trigger_data.*", 1);
	ch->SetBranchAddress("trigger_data.", &triggerData);

	RPRootDumpTrackInfo *rp_L_1_F = new RPRootDumpTrackInfo();
	RPRootDumpTrackInfo *rp_L_2_F = new RPRootDumpTrackInfo();

	RPRootDumpTrackInfo *rp_R_1_F = new RPRootDumpTrackInfo();
	RPRootDumpTrackInfo *rp_R_2_F = new RPRootDumpTrackInfo();

	// verticals in 45
	if (diagonal == d45b_56t || diagonal == ad45b_56b)
	{
		ch->SetBranchStatus("track_rp_5.*", 1);
		ch->SetBranchAddress("track_rp_5.", &rp_L_1_F);

		ch->SetBranchStatus("track_rp_25.*", 1);
		ch->SetBranchAddress("track_rp_25.", &rp_L_2_F);
	}
	
	if (diagonal == d45t_56b || diagonal == ad45t_56t)
	{
		ch->SetBranchStatus("track_rp_4.*", 1);
		ch->SetBranchAddress("track_rp_4.", &rp_L_1_F);

		ch->SetBranchStatus("track_rp_24.*", 1);
		ch->SetBranchAddress("track_rp_24.", &rp_L_2_F);
	}
	
	// verticals in 56
	if (diagonal == d45t_56b || diagonal == ad45b_56b)
	{
		ch->SetBranchStatus("track_rp_105.*", 1);
		ch->SetBranchAddress("track_rp_105.", &rp_R_1_F);

		ch->SetBranchStatus("track_rp_125.*", 1);
		ch->SetBranchAddress("track_rp_125.", &rp_R_2_F);
	}

	if (diagonal == d45b_56t || diagonal == ad45t_56t)
	{
		ch->SetBranchStatus("track_rp_104.*", 1);
		ch->SetBranchAddress("track_rp_104.", &rp_R_1_F);

		ch->SetBranchStatus("track_rp_124.*", 1);
		ch->SetBranchAddress("track_rp_124.", &rp_R_2_F);
	}

	// horizontals
	/*
	ch->SetBranchStatus("track_rp_23.*", 1);
	ch->SetBranchAddress("track_rp_23.", &rp_L_FH);

	ch->SetBranchStatus("track_rp_22.*", 1);
	ch->SetBranchAddress("track_rp_22.", &rp_L_NH);

	ch->SetBranchStatus("track_rp_122.*", 1);
	ch->SetBranchAddress("track_rp_122.", &rp_R_NH);

	ch->SetBranchStatus("track_rp_123.*", 1);
	ch->SetBranchAddress("track_rp_123.", &rp_R_FH);
	*/

	// ouput file
	string fn_out = string("distill_") + argv[1] + "_new.root";
	printf(">> output file: %s\n", fn_out.c_str());
	TFile *f_out = TFile::Open(fn_out.c_str(), "recreate");

	// set up output tree
	EventRed ev;
	TTree *outT = new TTree("distilled", "bla");

	outT->Branch("v_L_1_F", &ev.h.L_1_F.v); outT->Branch("x_L_1_F", &ev.h.L_1_F.x); outT->Branch("y_L_1_F", &ev.h.L_1_F.y);
	outT->Branch("v_L_2_F", &ev.h.L_2_F.v); outT->Branch("x_L_2_F", &ev.h.L_2_F.x); outT->Branch("y_L_2_F", &ev.h.L_2_F.y);

	outT->Branch("v_R_1_F", &ev.h.R_1_F.v); outT->Branch("x_R_1_F", &ev.h.R_1_F.x); outT->Branch("y_R_1_F", &ev.h.R_1_F.y);
	outT->Branch("v_R_2_F", &ev.h.R_2_F.v); outT->Branch("x_R_2_F", &ev.h.R_2_F.x); outT->Branch("y_R_2_F", &ev.h.R_2_F.y);

	outT->Branch("timestamp", &ev.timestamp);
	outT->Branch("run_num", &ev.run_num);
	outT->Branch("bunch_num", &ev.bunch_num);
	outT->Branch("event_num", &ev.event_num);
	outT->Branch("trigger_num", &ev.trigger_num);
	outT->Branch("trigger_bits", &ev.trigger_bits);

	// loop over the chain entries
	long int evi = 0;
	long int nDistilledEvents = 0;

	signal(SIGINT, SigIntHandler);

	for (; evi < ch->GetEntries() && !interrupt_loop; evi++)
	{
		ch->GetEvent(evi);

		ev.h.L_1_F.v = rp_L_1_F->valid; ev.h.L_1_F.x = rp_L_1_F->x; ev.h.L_1_F.y = rp_L_1_F->y;
		ev.h.L_2_F.v = rp_L_2_F->valid; ev.h.L_2_F.x = rp_L_2_F->x; ev.h.L_2_F.y = rp_L_2_F->y;
                                                                                         
		ev.h.R_1_F.v = rp_R_1_F->valid; ev.h.R_1_F.x = rp_R_1_F->x; ev.h.R_1_F.y = rp_R_1_F->y;
		ev.h.R_2_F.v = rp_R_2_F->valid; ev.h.R_2_F.x = rp_R_2_F->x; ev.h.R_2_F.y = rp_R_2_F->y;

		unsigned N_L = 0;
		if (ev.h.L_1_F.v) N_L++;
		if (ev.h.L_2_F.v) N_L++;

		unsigned N_R = 0;
		if (ev.h.R_1_F.v) N_R++;
		if (ev.h.R_2_F.v) N_R++;

		bool save = (N_L >= 1 && N_R >= 1);
		if (!save)
			continue;

		nDistilledEvents++;

		ev.timestamp = metaData->timestamp - timestamp0;
		ev.run_num = metaData->run_no;
		ev.bunch_num = triggerData->bunch_num;
		ev.event_num = triggerData->event_num;
		ev.trigger_num = triggerData->trigger_num;
		ev.trigger_bits = triggerData->input_status_bits;

		outT->Fill();
	}

	if (interrupt_loop)
		printf("WARNING: User interrupt!\n");

	printf(">> last event number: %li\n", evi);

	printf("\n");
	printf(">> distilled events: %li\n", nDistilledEvents);

	// save output tree
	outT->Write();

	delete f_out;
	return 0;
}
