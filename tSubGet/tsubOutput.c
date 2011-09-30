#include "tSubGet.h"

typedef struct _rTime{
	long h;
	long m;
	long s;
	long ms;
} rTime;

static void convertTime(long double timeInMs, rTime *rtOut){
	rtOut->h = (long)(timeInMs/(long double)3600000.0);
	timeInMs -= rtOut->h*3600000;
	rtOut->m = (long)(timeInMs/(long double)60000.0);
	timeInMs -= rtOut->m*60000;
	rtOut->s = (long)(timeInMs/(long double)1000.0);
	timeInMs -= rtOut->s*1000;
	rtOut->ms = (long)timeInMs;
}

int writeOutSubs(Details *uDets){
	FILE *fp;
	rTime st, et;
	Decoder d = uDets->po.d;

	unsigned  i;
	if (_wfopen_s(&fp,uDets->po.outFile,L"w"))
		return FALSE;

	for (i = 0; i < d.meta.capIdx;){
		unsigned tsIndex = d.caps[i].tsIndex;
		convertTime(d.ts[tsIndex].startTime,&st);
		convertTime(d.ts[tsIndex].endTime,&et);
		
		fwprintf(fp,L"%d\n",tsIndex);
		fwprintf(fp,L"%.2ld:%.2ld:%.2ld,%ld --> %.2ld:%.2ld:%.2ld,%ld\n",
					st.h, st.m, st.s, st.ms, et.h, et.m, et.s, et.ms);
		while (d.caps[i].tsIndex == tsIndex){
			if (uDets->pi.colouredOutput && d.caps[i].colour[0] != WHITE)
				fwprintf(fp,L"<font color=\"%s\">",colours[d.caps[i].colour[0]]);
			fwprintf(fp,L"%s",d.caps[i].text);
			if (uDets->pi.colouredOutput && d.caps[i].colour[0] != WHITE)
				fwprintf(fp,L"</font>");
			if (!d.caps[i].noBreak)
				fwprintf(fp,L"\n");
			i++;
		}
		fwprintf(fp,L"\n\n");
	}
	fclose(fp);
	return TRUE;	
}