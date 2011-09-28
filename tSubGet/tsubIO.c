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

static int initReader(IWMSyncReader **sr, WORD *streamNumber, wchar_t *filename){
	HRESULT hr;
	int outputCount, i;

	hr = WMCreateSyncReader(NULL,0,sr);
	if (FAILED(hr))
		return FALSE;
	hr = (*sr)->lpVtbl->Open(*sr,filename);
	if (FAILED(hr)){
		(*sr)->lpVtbl->Release(*sr);
		*sr = NULL;
		return FALSE;
	}

	(*sr)->lpVtbl->GetOutputCount(*sr,&outputCount);
	for (i = 0; i < outputCount; i++){
		int formatCount, j;

		(*sr)->lpVtbl->GetOutputFormatCount(*sr,i,&formatCount);
		for (j = 0; j < formatCount; j++){
			IWMOutputMediaProps *props;
			WM_MEDIA_TYPE *type;
			DWORD typeSize;

			hr = (*sr)->lpVtbl->GetOutputFormat(*sr,i,j,&props);
			if (FAILED(hr)){
				(*sr)->lpVtbl->Release(*sr);
				*sr = NULL;
				return FALSE;
			}

			props->lpVtbl->GetMediaType(props,NULL,&typeSize);
			type = (WM_MEDIA_TYPE*) calloc(1,typeSize);
			if (!type){
				props->lpVtbl->Release(props);
				(*sr)->lpVtbl->Release(*sr);
				*sr = NULL;
				return FALSE;
			}

			props->lpVtbl->GetMediaType(props,type,&typeSize);
			props->lpVtbl->Release(props);
			if (IsEqualGUID(&(type->majortype),&majorReq) &&
				IsEqualGUID(&(type->subtype),&subReq)){
				(*sr)->lpVtbl->GetStreamNumberForOutput(*sr,i,streamNumber);
				free(type);
				return TRUE;
			}
			free(type);
		} //End inner for loop for format count
	} //End out for loop for output count

	(*sr)->lpVtbl->Release(*sr);
	*sr = NULL;
	return FALSE;
}

int parseFile(Decoder *d, wchar_t *filename){
	int retval = FALSE;
	IWMSyncReader *sr;
	WORD streamNumber;
	INSSBuffer *sampleBuffer;
	QWORD sampleTime, sampleDuration;
	DWORD sampleFlags;
	
	if (!initReader(&sr,&streamNumber,filename))
		return FALSE;
	while (!FAILED(sr->lpVtbl->GetNextSample(sr,streamNumber,
					&sampleBuffer, &sampleTime, &sampleDuration, &sampleFlags,
					NULL,NULL))){
		sampleBuffer->lpVtbl->GetBufferAndLength(sampleBuffer,
			&(d->cSmp.buffer), &(d->cSmp.sLength));
		d->cSmp.cTime = (double)sampleTime/10000.0;
		retval = decodeSample(d);

		sampleBuffer->lpVtbl->Release(sampleBuffer);
		if (!retval)
			break;		
	}

	sr->lpVtbl->Release(sr);
	return retval;
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