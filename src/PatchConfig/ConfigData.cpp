#include "stdafx.h"
#include "ConfigData.h"
#include "PatchVersionInfo.h"
#include "Resource.h"

#define CONFIG_FILE "PAL3patch.conf"
#define MAXLINE 4096

static int is_spacechar(char ch)
{
	return !!strchr(" \t\n\v\f\r", ch);
}

// key => (sequence number, value)
static std::map<CString, std::pair<int, CString> > cfgdata;
static std::vector<std::pair<int, CString> > cfgcomments;

static int TryReadConfigFile()
{
	cfgdata.clear();
	cfgcomments.clear();
	int ret = 0;
	int seq = 0;
    FILE *fp = fopen(CONFIG_FILE, "r");
    if (!fp) goto done;
    char buf[MAXLINE];
    char *ptr;
    while (fgets(buf, sizeof(buf), fp)) {
		seq++;

        // ltrim the line
        for (ptr = buf; *ptr && is_spacechar(*ptr); ptr++);
        memmove(buf, ptr, strlen(ptr) + 1);
        
        // skip empty and comment lines
        if (!buf[0] || buf[0] == ';' || buf[0] == '#' || (buf[0] == '/' && buf[1] == '/')) {
			ptr = strrchr(buf, '\n');
			if (ptr) *ptr = '\0';
			if (buf[0] != ';') cfgcomments.push_back(std::make_pair(seq, CString(buf)));
			continue;
		}
        
        // remove '\n' and end of line
        ptr = strrchr(buf, '\n');
        if (ptr) *ptr = '\0';
        
        // parse 'key' and 'value'
        ptr = strchr(buf, '=');
        if (!ptr) goto done;
        *ptr = '\0';
        char *keystr = buf, *valstr = ptr + 1;
        
        // rtrim 'key'
        if (ptr > buf) ptr--;
        while (ptr >= buf && is_spacechar(*ptr)) *ptr-- = '\0';
        if (!*ptr) goto done;
        
        // ltrim 'value'
        while (*valstr && is_spacechar(*valstr)) valstr++;
        
        // save this config line to array
		cfgdata.insert(std::make_pair(CString(keystr), std::make_pair(seq, CString(valstr))));
    }

    ret = 1;
done:
	if (fp) fclose(fp);
	return ret;
}

int TryRebuildConfigFile()
{
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_DEFCONFIG), RT_RCDATA);
	if (!hRes) return 0;
	HGLOBAL hData = LoadResource(NULL, hRes);
	if (!hData) return 0;
	DWORD datalen = SizeofResource(NULL, hRes);
	if (!datalen) return 0;
	void *pData = LockResource(hData);
	if (!pData) return 0;

	FILE *fp = fopen(CONFIG_FILE, "wb");
	if (!fp) return 0;

	fwrite(pData, 1, datalen, fp);
	fclose(fp);

	return 1;
}

static int TryMatchConfigData()
{
	std::map<CString, std::pair<int, CString> >::iterator it;
	ConfigDescItem *pItem;
	for (pItem = ConfigDescList; pItem->level >= 0; pItem++) {
		if (pItem->key) {
			// no need to free 'pItem->pvalue', automaticly freed by std::map
			if ((it = cfgdata.find(CString(pItem->key))) != cfgdata.end()) {
				pItem->pvalue = &it->second.second;
			} else {
				return 0;
			}
		}
	}
	return 1;
}

int TryLoadConfigData()
{
	return TryReadConfigFile() && TryMatchConfigData();
}

int TrySaveConfigData()
{
	std::map<CString, std::pair<int, CString> >::iterator mapit;
	std::vector<std::pair<int, CString> >::iterator commentsit;
	FILE *fp = fopen(CONFIG_FILE, "w");
	if (!fp) return 0;
	
	// < <seq, type>, <key, val> >
	// type == 0, print "key=val\n"
	// type == 1, print "val\n"
	std::vector<std::pair<std::pair<int, int>, std::pair<CString, CString> > > cfgbuf;
	for (mapit = cfgdata.begin(); mapit != cfgdata.end(); mapit++) {
		cfgbuf.push_back(std::make_pair(std::make_pair(mapit->second.first, 0), std::make_pair(mapit->first, mapit->second.second)));
	}
	for (commentsit = cfgcomments.begin(); commentsit != cfgcomments.end(); commentsit++) {
		cfgbuf.push_back(std::make_pair(std::make_pair(commentsit->first, 1), std::make_pair(EMPTYSTR, commentsit->second)));
	}
	std::sort(cfgbuf.begin(), cfgbuf.end());

	SYSTEMTIME SystemTime;
    GetLocalTime(&SystemTime);
	fprintf(fp, "; generated by Patch Configure Utility %s (built on %s)\n", pVersionStr, pBuildDate);
	fprintf(fp, "; last modification: %04hu-%02hu-%02hu %02hu:%02hu:%02hu.%03hu\n", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds);
	std::vector<std::pair<std::pair<int, int>, std::pair<CString, CString> > >::iterator it;
	for (it = cfgbuf.begin(); it != cfgbuf.end(); it++) {
		LPCTSTR keystr = it->second.first;
		LPCTSTR valstr = it->second.second;
#if defined(_UNICODE)
		int keylen = WideCharToMultiByte(CP_ACP, 0, keystr, -1, NULL, 0, NULL, NULL);
		int vallen = WideCharToMultiByte(CP_ACP, 0, valstr, -1, NULL, 0, NULL, NULL);
		if (!keylen || !vallen) break;
		char *keybuf = (char *) malloc(keylen);
		char *valbuf = (char *) malloc(vallen);
		WideCharToMultiByte(CP_ACP, 0, keystr, -1, keybuf, keylen, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, valstr, -1, valbuf, vallen, NULL, NULL);
		if (it->first.second == 0) {
			fprintf(fp, "%s=%s\n", keybuf, valbuf);
		} else {
			fprintf(fp, "%s\n", valbuf);
		}
		free(keybuf);
		free(valbuf);
#elif defined(_MBCS)
		if (it->first.second == 0) {
			fprintf(fp, "%s=%s\n", keystr, valstr);
		} else {
			fprintf(fp, "%s\n", valstr);
		}
#else
#error 
#endif
	}
	if (fp) fclose(fp);
	return 1;
}