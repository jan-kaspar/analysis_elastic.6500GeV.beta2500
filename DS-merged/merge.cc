#include "TFile.h"
#include "TH1D.h"

#include <vector>
#include <string>
#include <map>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------------------------------

struct shist
{
	TH1D *hist;
	double scale;
	shist(TH1D *_h, double _s) : hist(_h), scale(_s) {}
};

//----------------------------------------------------------------------------------------------------

TH1D* Merge(const vector<shist> &hists, bool sumBins = true)
{
	// prepare merged histogram
	TH1D *m = new TH1D(*hists[0].hist);

	// merge number of entries
	unsigned int entries = 0;
	for (unsigned int hi = 0; hi < hists.size(); hi++)
		entries += hists[hi].hist->GetEntries();

	m->SetEntries(entries);

	// merge bins
	for (int bi = 1; bi <= m->GetNbinsX(); bi++)
	{
		if (sumBins)
		{
			// sum bins
			double Sv = 0., Su2 = 0.;
			for (unsigned int hi = 0; hi < hists.size(); hi++)
			{
				double v = hists[hi].hist->GetBinContent(bi);
				double u = hists[hi].hist->GetBinError(bi) * hists[hi].scale;

				Sv += v;
				Su2 += u * u;
			}

			double v = Sv;
			double u = sqrt(Su2);

			m->SetBinContent(bi, v);
			m->SetBinError(bi, u);
		} else {
			// average bins
			double Svw = 0., Sw = 0.;
			for (unsigned int hi = 0; hi < hists.size(); hi++)
			{
				double v = hists[hi].hist->GetBinContent(bi);
				double e = hists[hi].hist->GetBinError(bi) * hists[hi].scale;
				double w = (e > 0.) ? 1./e/e : 0.;

				Sw += w;
				Svw += v * w;
			}

			double v = (Sw > 0.) ? Svw / Sw : 0.;
			double e = (Sw > 0.) ? 1. / sqrt(Sw) : 0.;

			// TODO
			/*
			double S1 = 0., Sv = 0., Su2 = 0.;
			for (unsigned int hi = 0; hi < hists.size(); hi++)
			{
				double v = hists[hi].hist->GetBinContent(bi);
				double u = hists[hi].hist->GetBinError(bi) * hists[hi].scale;

				S1 += 1.;
				Sv += v;
				Su2 += u*u;
			}

			double v = (S1 > 0.) ? Sv / S1 : 0.;
			double e = (S1 > 0.) ? sqrt(Su2) / S1 : 0.;
			*/

			m->SetBinContent(bi, v);
			m->SetBinError(bi, e);
		}
	}

	return m;
}

//----------------------------------------------------------------------------------------------------

struct Entry
{
	string input;
	double stat_unc_scale;
	string output;
	bool merge;

	Entry(const string &_i, double _sus, const string &_o, bool _m) : input(_i), stat_unc_scale(_sus), output(_o), merge(_m) {}
};

//----------------------------------------------------------------------------------------------------

int main()
{
	vector<Entry> entries;
	entries.push_back(Entry("DS-fill5313", 1., "DS-fill5313", true));
	entries.push_back(Entry("DS-fill5314", 1., "DS-fill5314", true));
	entries.push_back(Entry("DS-fill5317", 1., "DS-fill5317", true));
	entries.push_back(Entry("DS-fill5321", 1., "DS-fill5321", true));

	entries.push_back(Entry("DS-firstParts", 1., "DS-firstParts", false));
	entries.push_back(Entry("DS-lastParts", 1., "DS-lastParts", false));

	vector<string> diagonals;
	diagonals.push_back("45b_56t");
	diagonals.push_back("45t_56b");

	vector<string> binnings;
	//binnings.push_back("ub");
	binnings.push_back("ob-1-20-0.05");
	binnings.push_back("ob-2-10-0.05");
	binnings.push_back("ob-3-5-0.05");

	// prepare output
	TFile *f_out = new TFile("merged.root", "recreate");

	// loop over binnings
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		printf("\t%s\n", binnings[bi].c_str());

		TDirectory *binningDir = f_out->mkdir(binnings[bi].c_str());

		// list of histograms for final merge
		vector<shist> full_list_L, full_list_no_L;				

		// map: diagonal --> list of inputs
		map<string, vector<shist> > full_map_L, full_map_no_L;

		for (unsigned int ei = 0; ei < entries.size(); ei++)
		{
			printf("\t\t%s\n", entries[ei].output.c_str());

			TDirectory *datasetDir = binningDir->mkdir(entries[ei].output.c_str());

			vector<shist> ds_list_L, ds_list_no_L;
			for (unsigned int dgni = 0; dgni < diagonals.size(); dgni++)
			{
				printf("\t\t\t%s\n", diagonals[dgni].c_str());

				string fn = "../" + entries[ei].input + "/distributions_"+diagonals[dgni]+".root";
				TFile *f_in = TFile::Open(fn.c_str());
				if (!f_in)
				{
					printf("ERROR: File `%s' cannot be opened.\n", fn.c_str());
					return 1;
				}

				//string on = "normalization/"+binnings[bi]+"/h_t_normalized";
				string on = "normalization+unfolding/"+binnings[bi]+"/h_t_normalized_unsmeared";
				TH1D *h_norm_L = (TH1D *) f_in->Get(on.c_str());
				if (!h_norm_L)
				{
					printf("ERROR: Object `%s' cannot be loaded from file `%s'.\n", on.c_str(), fn.c_str());
					return 2;
				}

				on = "normalization/"+binnings[bi]+"/h_t_normalized_no_L";
				TH1D *h_norm_no_L = (TH1D *) f_in->Get(on.c_str());
				if (!h_norm_no_L)
				{
					printf("ERROR: Object `%s' cannot be loaded from file `%s'.\n", on.c_str(), fn.c_str());
					return 2;
				}

				h_norm_L->SetName("h_dsdt");
				h_norm_no_L->SetName("h_dNdt");

				shist sc_hist_L(h_norm_L, entries[ei].stat_unc_scale);
				shist sc_hist_no_L(h_norm_no_L, entries[ei].stat_unc_scale);

				ds_list_L.push_back(sc_hist_L);
				ds_list_no_L.push_back(sc_hist_no_L);

				if (entries[ei].merge)
				{
					full_list_L.push_back(sc_hist_L);
					full_list_no_L.push_back(sc_hist_no_L);

					full_map_L[diagonals[dgni]].push_back(sc_hist_L);
					full_map_no_L[diagonals[dgni]].push_back(sc_hist_no_L);
				}

				gDirectory = datasetDir->mkdir(diagonals[dgni].c_str());
				h_norm_L->Write();
				h_norm_no_L->Write();
			}

			// save histogram merged from all inputs of a dataset
			gDirectory = datasetDir->mkdir("combined");
			Merge(ds_list_L, false)->Write();
			Merge(ds_list_no_L, true)->Write();
		}

		// save merged histograms
		TDirectory *mergedDir = binningDir->mkdir("merged");

		for (map<string, vector<shist> >::iterator it = full_map_L.begin(); it != full_map_L.end(); ++it)
		{
			gDirectory = mergedDir->mkdir(it->first.c_str());
			Merge(it->second, false)->Write();

			auto it_no_L = full_map_no_L.find(it->first);
			Merge(it_no_L->second, true)->Write();
		}

		gDirectory = mergedDir->mkdir("combined");
		Merge(full_list_L, false)->Write();
		Merge(full_list_no_L, true)->Write();
	}

	delete f_out;

	return 0;
}
