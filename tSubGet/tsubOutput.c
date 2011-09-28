#include "tsubStreamer.h"
#include "tsubIO.h"

static void convertTime(double timeInMs, rTime *rtOut){
	rtOut->h = (long)(timeInMs/3600000.0);
	timeInMs -= rtOut->h*3600000;
	rtOut->m = (long)(timeInMs/60000.0);
	timeInMs -= rtOut->m*60000;
	rtOut->s = (long)(timeInMs/1000.0);
	timeInMs -= rtOut->s*1000;
	rtOut->ms = (long)timeInMs;
}

void getSubFilename(wchar_t *inputFN, wchar_t *outputFN, int szOut){
	wchar_t *ptr;
	wcsncpy_s(outputFN,szOut-SUB_EXT_SZ,inputFN,_TRUNCATE);
	ptr = wcsrchr(outputFN,L'.');
	if (ptr) *ptr = L'\0';
	wcsncat_s(outputFN,szOut,SUB_EXT,_TRUNCATE);
}

int writeOutSubs(Decoder d, wchar_t *subFilename){
	FILE *fp;
	rTime st, et;
	int i,j,capCounter = 1;

	if (_wfopen_s(&fp,subFilename,L"w"))
		return FALSE;

	for(i = 0; i < d.cCollPos;){
		j = i;
		convertTime(d.coll[j].startTime,&st);

		for (; j < d.cCollPos && d.coll[j].isLinked; j++);
		//The collPos can actually be one off if the last collection
		//isn't finalised due to stream being 'cut off'
		if (j == d.cCollPos && d.cCollPos ==d.cCollSize){
			fclose(fp);
			return FALSE;
		}

		convertTime(d.coll[j].endTime,&et);
		fwprintf(fp,L"%d\n",capCounter);
		fwprintf(fp,L"%.2ld:%.2ld:%.2ld,%ld --> %.2ld:%.2ld:%.2ld,%ld\n",
					st.h, st.m, st.s, st.ms, et.h, et.m, et.s, et.ms);
		for (; i <= j; i++){
			int k; 
			for (k = 0; k < d.coll[i].cCapPos; k++){
				if (d.coll[i].cap[k].fgCol != WHITE)
					fwprintf(fp,L"<font color=\"%s\">",colours[d.coll[i].cap[k].fgCol]);
				fwprintf(fp,L"%s",d.coll[i].cap[k].text);
				if (d.coll[i].cap[k].fgCol != WHITE)
					fwprintf(fp,L"</font>");
				if (!d.coll[i].cap[k].noBreak)
					fwprintf(fp,L"\n");
			}
		}
		fwprintf(fp,L"\n\n");
		capCounter++;
	}
	fclose(fp);
	return TRUE;
}