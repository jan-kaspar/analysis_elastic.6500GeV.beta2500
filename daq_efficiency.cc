#include "common_definitions.h"
#include "common_algorithms.h"
#include "parameters.h"
#include "common.h"

#include "TFile.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TChain.h"

#include <cmath>

//----------------------------------------------------------------------------------------------------

struct PeriodIndex
{
	unsigned int run;
	signed int idx;
};

//----------------------------------------------------------------------------------------------------

bool operator < (const PeriodIndex &a, const PeriodIndex &b)
{
	if (a.run < b.run)
		return true;
	if (a.run > b.run)
		return false;

	if (a.idx < b.idx)
		return true;

	return false;
}

//----------------------------------------------------------------------------------------------------

struct PeriodData
{
	unsigned int timestamp_min=0, timestamp_max=0;
	unsigned int event_num_min=0, event_num_max=0;
	unsigned int trigger_num_min=0, trigger_num_max=0;
};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if (argc != 2)
		return 1;

	// init diagonal settings
	Init(argv[1]);
	if (diagonal == dCombined || diagonal == ad45b_56b || diagonal == ad45t_56t)
		return rcIncompatibleDiagonal;

	// get input data
	TChain *ch_in = new TChain("distilled");
	printf(">> input chain\n");
	for (const auto &ntupleDir : distilledNtuples)
	{
		string f = ntupleDir + "/distill_" + argv[1] + ".root";
		printf("    %s\n", f.c_str());
		ch_in->Add(f.c_str());
	}
	printf("%llu entries\n", ch_in->GetEntries());

	// set up branches
	EventRed ev;
	
	ch_in->SetBranchAddress("timestamp", &ev.timestamp);
	ch_in->SetBranchAddress("run_num", &ev.run_num);
	ch_in->SetBranchAddress("event_num", &ev.event_num);
	ch_in->SetBranchAddress("trigger_num", &ev.trigger_num);
	
	// prepare output
	TFile *f_out = new TFile((string("daq_efficiency_") + argv[1] + ".root").c_str(), "recreate");

	// settings
	unsigned int period_length = 5 * 60;	// s

	// extract period data
	map<PeriodIndex, PeriodData> periods;

	for (int ev_idx = 0; ev_idx < ch_in->GetEntries(); ev_idx += 1)
	{
		ch_in->GetEntry(ev_idx);

		// get period index
		PeriodIndex idx;
		idx.run = ev.run_num / 100000;
		idx.idx = ev.timestamp / period_length;

		// update period data
		auto it = periods.find(idx);
		if (it == periods.end())
		{
			PeriodData d;
			d.timestamp_min = d.timestamp_max = ev.timestamp;
			d.event_num_min = d.event_num_max = ev.event_num;
			d.trigger_num_min = d.trigger_num_max = ev.trigger_num;
			periods.insert({idx, d});
		} else {
			PeriodData &d = it->second;

			d.timestamp_min = min(d.timestamp_min, ev.timestamp);
			d.timestamp_max = max(d.timestamp_min, ev.timestamp);

			d.event_num_min = min(d.event_num_min, ev.event_num);
			d.event_num_max = max(d.event_num_min, ev.event_num);

			d.trigger_num_min = min(d.trigger_num_min, ev.trigger_num);
			d.trigger_num_max = max(d.trigger_num_min, ev.trigger_num);
		}
	}

	// make plots
	TH1D *h_eff = new TH1D("h_eff", ";DAQ efficiency", 100, 0.8, 1.0);

	TGraphErrors *g_eff_vs_time = new TGraphErrors();
	g_eff_vs_time->SetName("g_eff_vs_time");

	for (const auto &p : periods)
	{
		// skip periods with too little data
		unsigned int duration = p.second.timestamp_max - p.second.timestamp_min;
		if (duration < 10)
			continue;

		// calculate efficiecy
		double eff = double(p.second.event_num_max - p.second.event_num_min) / double(p.second.trigger_num_max - p.second.trigger_num_min);
		double eff_unc = 0.;

		// fill plots
		h_eff->Fill(eff);

		double time = (double(p.second.timestamp_max) + double(p.second.timestamp_min)) / 2.;

		unsigned int g_idx = g_eff_vs_time->GetN();
		g_eff_vs_time->SetPoint(g_idx, time, eff);
		g_eff_vs_time->SetPointError(g_idx, double(duration) / 2., eff_unc);
	}

	h_eff->Write();
	g_eff_vs_time->Write();

	// clean up
	delete f_out;

	return 0;
}
