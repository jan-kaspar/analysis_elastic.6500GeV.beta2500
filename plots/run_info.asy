//----------------------------------------------------------------------------------------------------

transform swToHours = scale(1/3600, 1);

//----------------------------------------------------------------------------------------------------

string run_datasets[];
int runs[];
real ts_from[], ts_to[];
pen colors[];

void AddRun(string ds, int r, real f, real t, pen p = yellow)
{
	run_datasets.push(ds);
	runs.push(r);
	ts_from.push(f);
	ts_to.push(t);
	colors.push(p);
}

string fill_str = "5313";
AddRun(fill_str, 10190, 1665, 4138);
AddRun(fill_str, 10192, 7645, 10079);
AddRun(fill_str, 10194, 10565, 13637);
AddRun(fill_str, 10196, 13998, 16853);
AddRun(fill_str, 10198, 17222, 20073);
AddRun(fill_str, 10200, 20493, 23493);
AddRun(fill_str, 10202, 23964, 26428);
AddRun(fill_str, 10204, 27043, 30240);
AddRun(fill_str, 10206, 30873, 31984);

string fill_str = "5314";
AddRun(fill_str, 10208, 51423, 53711);
AddRun(fill_str, 10209, 55244, 56660);
AddRun(fill_str, 10210, 56692, 57517);
AddRun(fill_str, 10212, 58361, 60328);
AddRun(fill_str, 10214, 61353, 63341);
AddRun(fill_str, 10216, 64504, 66810);
AddRun(fill_str, 10218, 67268, 70360);
AddRun(fill_str, 10220, 71267, 74558);
AddRun(fill_str, 10222, 75037, 78750);
AddRun(fill_str, 10224, 79178, 81466);

string fill_str = "5317.1";
AddRun(fill_str, 10225, 104904, 107576);
AddRun(fill_str, 10227, 108092, 110492);
AddRun(fill_str, 10229, 110973, 113545);
AddRun(fill_str, 10231, 113958, 117038);
AddRun(fill_str, 10233, 117429, 118087);
AddRun(fill_str, 10234, 118483, 120664);
AddRun(fill_str, 10236, 121236, 124279);
AddRun(fill_str, 10238, 124698, 127943);
AddRun(fill_str, 10240, 128363, 131944);
AddRun(fill_str, 10242, 132283, 134399);
AddRun(fill_str, 10243, 134747, 136787);
AddRun(fill_str, 10245, 137191, 137510);
AddRun(fill_str, 10246, 137714, 137714);
AddRun(fill_str, 10248, 138073, 138548);
AddRun(fill_str, 10249, 138796, 139017);
AddRun(fill_str, 10250, 139309, 141834);
AddRun(fill_str, 10252, 142280, 148235);
AddRun(fill_str, 10254, 148585, 153714);
string fill_str = "5317.2";
AddRun(fill_str, 10256, 154377, 160135);
AddRun(fill_str, 10258, 160534, 165371);
AddRun(fill_str, 10260, 165742, 170943);
AddRun(fill_str, 10262, 171539, 176687);
AddRun(fill_str, 10264, 177147, 182520);
AddRun(fill_str, 10266, 182991, 188431);
AddRun(fill_str, 10268, 188972, 194510);
AddRun(fill_str, 10270, 194861, 200947);
AddRun(fill_str, 10272, 201335, 206348);

string fill_str = "5321";
AddRun(fill_str, 10281, 265690, 269310);
AddRun(fill_str, 10283, 269677, 272644);
AddRun(fill_str, 10285, 273040, 276181);
AddRun(fill_str, 10286, 276382, 277270);
AddRun(fill_str, 10288, 277720, 280550);
AddRun(fill_str, 10289, 280941, 281584);
AddRun(fill_str, 10292, 282770, 284299);
AddRun(fill_str, 10293, 284471, 285305);
AddRun(fill_str, 10294, 286084, 287932);
AddRun(fill_str, 10295, 288101, 294701);
AddRun(fill_str, 10297, 295254, 298099);
AddRun(fill_str, 10298, 298394, 300067);
AddRun(fill_str, 10299, 300482, 300909);
AddRun(fill_str, 10301, 301563, 302882);
AddRun(fill_str, 10302, 303158, 303626);
AddRun(fill_str, 10303, 304070, 306709);
AddRun(fill_str, 10305, 307097, 309741);


//----------------------------------------------------------------------------------------------------

void DrawRunBands(string ds_filter="", real y_min=0, real y_max=0, bool details=true)
{
	for (int i : runs.keys)
	{
		if (ds_filter != "")
		{
			if (find(run_datasets[i], ds_filter) == -1)
				continue;
		}

		//yaxis(XEquals(ts_from[i]/3600, false), dashed);
		//yaxis(XEquals(ts_to[i]/3600, false), dashed);
		real x_min = ts_from[i]/3600, x_max = ts_to[i]/3600;

		pen p = (details) ? colors[i]+opacity(0.3) : yellow+opacity(0.3);
		filldraw((x_min, y_min)--(x_max, y_min)--(x_max, y_max)--(x_min, y_max)--cycle, p, nullpen);

		if (details)
			label(format("{\SmallerFonts\it %u}", runs[i]), ((x_min + x_max)/2, y_max), S);
	}
}

//----------------------------------------------------------------------------------------------------

void DrawRunBoundaries(string ds_filter="")
{
	for (int i : runs.keys)
	{
		if (ds_filter != "")
			if (run_datasets[i] != ds_filter)
				continue;

		yaxis(XEquals(ts_from[i]/3600, false), dashed);
		yaxis(XEquals(ts_to[i]/3600, false), dashed);
	}
}
