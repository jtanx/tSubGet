#include "tSubInternalV2.h"

/*
	A proposed strategy for the intermediary storage of teletext subtitles.
	Top level: Have an array of captions, where each caption corresponds to
	a set display time. Within each caption, have an array of portions of text.

	Each portion of text has specific characteristics, such as a different
	position, or different style.
*/

TSG_API_DECLARE(HRESULT) TSG_Init(wchar_t *fileIn, TSG_Parser **pRet) {
	if (!pRet || !fileIn) {
		return E_INVALIDARG;
	} else {
		AM_MEDIA_TYPE am = {0};
		NGC_Info *reader;
		TSG_Parser *parser;
		HRESULT hr;

		am.majortype = MEDIATYPE_MSTVCaption;
		if (FAILED(hr = NGC_Init(fileIn, &am, &reader))) {
			return hr;
		} else if (!(parser = calloc(1, sizeof(TSG_Parser)))) {
			NGC_Free(&reader);
			return E_OUTOFMEMORY;
		} else if (!ab_init(&parser->captionsBlock, 0, sizeof(CaptionsBlock))) {
			NGC_Free(&reader);
			free(parser);
			return E_OUTOFMEMORY;
		}
		parser->fileReader = reader;

		//Hook the parser into the sample grabber callback
		NGC_SetSampleCallback(parser->fileReader, parseSampleV2 , parser);
		
		*pRet = parser;
		return S_OK;
	}
}

TSG_API_DECLARE(void) TSG_SetInfoCallback (
	TSG_Parser *parser, TSG_Info callback)
{
	if (parser) {
		parser->infoCallback = callback;
	}
}

TSG_API_DECLARE(void) TSG_SetProgressCallback(
	TSG_Parser *parser, TSG_Progress progress, void *userData) 
{
	if (parser) {
		NGC_SetProgressCallback(parser->fileReader, progress, userData);
	}
}

TSG_API_DECLARE(HRESULT) TSG_Run(
	TSG_Parser *parser, unsigned pageNumber, TSG_LangID langId) 
{
	if (parser && IsLangId(langId) && IsPageNo(pageNumber)) {
		return NGC_Run(parser->fileReader);
	}
	return E_INVALIDARG;
}

TSG_API_DECLARE(void) TSG_Abort(TSG_Parser *parser) {
	if (parser) {
		NGC_Abort(parser->fileReader);
	}
}

TSG_API_DECLARE(void) TSG_Free(TSG_Parser **parser) {
	if (parser && *parser) {
		NGC_Free(&(*parser)->fileReader);
		//Need to free captions shit
		free(*parser);
		*parser = NULL;
	}
}

TSG_API_DECLARE(void) TSG_GetErrorString(HRESULT hr, wchar_t *buf, size_t numChars) {
	if (!buf)
		return;
	switch (hr) {
		case NGC_NO_INPUT_STREAM:
			wcsncpy_s(buf, numChars, L"The input file has no teletext stream.", _TRUNCATE);
			break;
		case NGC_UNKNOWN_COM_ERROR:
			wcsncpy_s(buf, numChars, L"An unexpected COM error occurred in NullGrabber.", _TRUNCATE);
			break;
		case NGC_UNEXPECTED_RUN_ERROR:
			wcsncpy_s(buf, numChars, L"NullGrabber's DirectShow graph stopped unexpectedly.", _TRUNCATE);
			break;
		//Include dxerr.h/dxerr.lib instead? effort
		case VFW_E_CANNOT_LOAD_SOURCE_FILTER: case VFW_E_UNKNOWN_FILE_TYPE:
			wcsncpy_s(buf, numChars, L"The input file is not supported.", _TRUNCATE);
			break;
		case VFW_E_NOT_FOUND:
			wcsncpy_s(buf, numChars, L"The input file was not found.", _TRUNCATE);
			break;
		default:
			if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, buf, numChars, NULL)) 
				wcsncpy_s(buf, numChars, L"Unknown error code.", _TRUNCATE);
			break;
	}
}