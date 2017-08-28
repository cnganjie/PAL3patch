#include "common.h"

// language, see patch_setlocale.c for details
unsigned system_codepage;
unsigned target_codepage;

// game locale information, see GAME_LOCALE_XXX for details
int game_locale;

// CHS
static const wchar_t wstr_about_title_CHS[] = L"����";
static const wchar_t wstr_about_text_CHS[] = 
    L"��ӭʹ�á��ɽ������������ֱ��ʲ��� %hs\n" 
    L"\n"
    L"����������������Ϸ�ķֱ���\n"
    L"ͬʱ�޸�һЩ��Ϸ�����С����\n"
    L"����������һЩʵ�õ�С����\n" 
    L"��ϸ��Ϣ��ʹ�÷�����μ�����˵���ļ�\n"
    L"\n"
    L"\n"
    L"���������ÿ������ʱ��������Ϣ\n"
    L"��ʹ�ò������ù��ߣ�������ʾ���ڡ���Ϊ�����á�\n" 
    L"\n"
    L"\n"
    L"������վ:\n"
    L"  https://pal3.zbyzbyzby.com\n"
    L"\n"
    L"������Ϣ:\n%hs"
;

static const wchar_t wstr_confirmquit_text_CHS[] = L"��ȷ��Ҫ�˳���Ϸ��\nδ�������Ϸ���Ƚ��ᶪʧ��";
static const wchar_t wstr_confirmquit_title_CHS[] = L"�˳�";

static const char ftfont_filename_CHS[] = "simsun.ttc";
static const int ftfont_index_CHS = 0;
static const DWORD defaultfont_charset_CHS = GB2312_CHARSET;
static const int defaultfont_bold_CHS = 48;

static const wchar_t wstr_defaultfont_CHS[] = L"����";
static const wchar_t wstr_gameloading_CHS[] = L"��Ϸ�����У����Ժ򡭡�";

static const wchar_t wstr_cantsavereg_text_CHS[] = L"�޷�����ע������ݵ��浵�ļ��У����ִ浵���ݿ��ܻᶪʧ��\n��ȷ����Ϸ�Դ浵�ļ����п�дȨ�ޡ�";
static const wchar_t wstr_cantsavereg_title_CHS[] = L"ע����ض���";

static const wchar_t wstr_nocfgfile_text_CHS[] = L"�޷���ȡ���������ļ��������С��������ù��ߡ�������Ĭ�������ļ���";
static const wchar_t wstr_nocfgfile_title_CHS[] = L"�޷���������";

static const wchar_t wstr_badcfgfile_text_CHS[] = L"���������ļ��𻵡������С��������ù��ߡ���ʹ�á�ʵ�ù��ߡ����ָ�Ĭ�����á���������д�����ļ���";
static const wchar_t wstr_badcfgfile_title_CHS[] = L"�޷���������";


static const wchar_t wstr_pluginerr_notfound_text_CHS[] = L"�Ҳ���ָ���Ĳ��ģ�顰%s����";
static const wchar_t wstr_pluginerr_loadfailed_text_CHS[] = L"�޷����ز��ģ�顰%s����";
static const wchar_t wstr_pluginerr_noentry_text_CHS[] = L"�޷��ڡ�%s�����ҵ����ģ����ڵ㡰%hs������ȷ�����Ƿ�Ϊ�Ϸ��Ĳ��ģ�顣";
static const wchar_t wstr_pluginerr_initfailed_text_CHS[] = L"�޷���ʼ�����ģ�顰%s��������ֵΪ %d��";
static const wchar_t wstr_pluginerr_title_CHS[] = L"�޷����ز��";


// CHT
static const char ftfont_filename_CHT[] = "mingliu.ttc";
static const int ftfont_index_CHT = 0;
static const DWORD defaultfont_charset_CHT = CHINESEBIG5_CHARSET;
static const int defaultfont_bold_CHT = 32;

static const wchar_t wstr_defaultfont_CHT[] = L"�����w";

// FIXME: add more translations




// string pointers and default values

const wchar_t *wstr_about_title;
const wchar_t *wstr_about_text;
const wchar_t *wstr_confirmquit_text;
const wchar_t *wstr_confirmquit_title;
const char *ftfont_filename;
int ftfont_index;
const wchar_t *wstr_defaultfont;
DWORD defaultfont_charset;
int defaultfont_bold;
const wchar_t *wstr_gameloading;
const wchar_t *wstr_cantsavereg_text;
const wchar_t *wstr_cantsavereg_title;
const wchar_t *wstr_nocfgfile_text;
const wchar_t *wstr_nocfgfile_title;
const wchar_t *wstr_badcfgfile_text;
const wchar_t *wstr_badcfgfile_title;
const wchar_t *wstr_pluginerr_notfound_text;
const wchar_t *wstr_pluginerr_loadfailed_text;
const wchar_t *wstr_pluginerr_noentry_text;
const wchar_t *wstr_pluginerr_initfailed_text;
const wchar_t *wstr_pluginerr_title;







static int detect_game_locale();

#define IMPORT_LOCALE_ITEM(lang, symbol) ((symbol) = (CONCAT3(symbol, _, lang)))

void init_locale_early()
{
    // init codepage
    // PATCHSET 'setlocale' may overwrite target_codepage
    target_codepage = system_codepage = GetACP();

    // detect game locale
    game_locale = detect_game_locale();
    
    
    // init early strings
    IMPORT_LOCALE_ITEM(CHS, wstr_nocfgfile_text);
    IMPORT_LOCALE_ITEM(CHS, wstr_nocfgfile_title);
    IMPORT_LOCALE_ITEM(CHS, wstr_badcfgfile_text);
    IMPORT_LOCALE_ITEM(CHS, wstr_badcfgfile_title);
    
    if (target_codepage == CODEPAGE_CHT) { // CHT
        // FIXME
    }
}

void init_locale()
{
    // no translations yet
    IMPORT_LOCALE_ITEM(CHS, wstr_about_title);
    IMPORT_LOCALE_ITEM(CHS, wstr_about_text);
    IMPORT_LOCALE_ITEM(CHS, wstr_confirmquit_text);
    IMPORT_LOCALE_ITEM(CHS, wstr_confirmquit_title);
    IMPORT_LOCALE_ITEM(CHS, ftfont_filename);
    IMPORT_LOCALE_ITEM(CHS, ftfont_index);
    IMPORT_LOCALE_ITEM(CHS, wstr_defaultfont);
    IMPORT_LOCALE_ITEM(CHS, defaultfont_charset);
    IMPORT_LOCALE_ITEM(CHS, defaultfont_bold);
    IMPORT_LOCALE_ITEM(CHS, wstr_gameloading);
    IMPORT_LOCALE_ITEM(CHS, wstr_cantsavereg_text);
    IMPORT_LOCALE_ITEM(CHS, wstr_cantsavereg_title);
    IMPORT_LOCALE_ITEM(CHS, wstr_pluginerr_notfound_text);
    IMPORT_LOCALE_ITEM(CHS, wstr_pluginerr_loadfailed_text);
    IMPORT_LOCALE_ITEM(CHS, wstr_pluginerr_noentry_text);
    IMPORT_LOCALE_ITEM(CHS, wstr_pluginerr_initfailed_text);
    IMPORT_LOCALE_ITEM(CHS, wstr_pluginerr_title);

    if (target_codepage == CODEPAGE_CHT) { // CHT
        IMPORT_LOCALE_ITEM(CHT, ftfont_filename);
        IMPORT_LOCALE_ITEM(CHT, ftfont_index);
        IMPORT_LOCALE_ITEM(CHT, wstr_defaultfont);
        IMPORT_LOCALE_ITEM(CHT, defaultfont_charset);
        IMPORT_LOCALE_ITEM(CHT, defaultfont_bold);
        // FIXME
    }
}









static int detect_game_locale()
{
    // read compressed data of "basedata.cpk\datascript\lang.txt"
    // and use magic string to determine game locale
    
    const char *CHS_magic = "\x41\x70\x70\x4E\x61\x6D\x65\x09\x09\x24\xCF\xC9\xBD\xA3\xC6\xE6\xCF\xC0\xB4\xAB\x33\x26";
    const char *CHT_magic = "\x41\x70\x70\x4E\x61\x6D\x65\x09\x09\x24\xA5\x50\xBC\x43\xA9\x5F\xAB\x4C\xB6\xC7\x33\x26";
    size_t CHS_magic_len = strlen(CHS_magic);
    size_t CHT_magic_len = strlen(CHT_magic);
    ULONG key_CRC = 0xCB283888; // equals gbCrc32Compute("datascript\\lang.txt"), but we can't call gbCrc32Compute() at this time
    
    int result = GAME_LOCALE_UNKNOWN;

    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMapFile = NULL;
    LPVOID lpMapAddress = NULL;

    unsigned i;
    int left, right, mid;
    ULONG base, skip;
    SYSTEM_INFO SysInfo;
    DWORD dwSysGran;
    struct CPKHeader cpkhdr;
    struct CPKTable *cpktbl;
    struct CPKTable cpkitem;
    void *pdata;
    ULONG datasz;
    
    // create file handle
    hFile = CreateFile("basedata\\basedata.cpk", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
    if (hFile == INVALID_HANDLE_VALUE) goto done;
    
    // create file mapping handle
    hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!hMapFile) goto done;
    
    // get memory allocation granularity
    GetSystemInfo(&SysInfo);
    dwSysGran = SysInfo.dwAllocationGranularity;
    
    // map CPK header
    lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, sizeof(struct CPKHeader));
    if (!lpMapAddress) goto done;
    
    // read CPK header
    memcpy(&cpkhdr, lpMapAddress, sizeof(cpkhdr));
    
    // map CPK index table
    UnmapViewOfFile(lpMapAddress);
    lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, sizeof(struct CPKHeader) + sizeof(struct CPKTable) * cpkhdr.dwMaxFileNum);
    if (!lpMapAddress) goto done;
    cpktbl = PTRADD(lpMapAddress, sizeof(struct CPKHeader));
    
    // search CPK table entry
    left = 0;
    right = cpkhdr.dwValidTableNum;
	while (1) {
        if (left == right) goto done;
        mid = left + (right - left) / 2;
        if (cpktbl[mid].dwCRC == key_CRC && (cpktbl[mid].dwFlag & 0x1) && !(cpktbl[mid].dwFlag & 0x10)) {
            break;
        }
        if (left + 1 == right) goto done;
        if (key_CRC >= cpktbl[mid].dwCRC) {
            left = mid;
        } else {
            right = mid;
        }
    }
	memcpy(&cpkitem, &cpktbl[mid], sizeof(cpkitem));
	
	// map file data
	base = ROUND_DOWN(cpkitem.dwStartPos, dwSysGran);
	skip = cpkitem.dwStartPos - base;
    UnmapViewOfFile(lpMapAddress);
    lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, base, skip + cpkitem.dwPackedSize);
    if (!lpMapAddress) goto done;
    pdata = PTRADD(lpMapAddress, skip);
    datasz = cpkitem.dwPackedSize;
	
	// process data
	for (i = 0; i < datasz; i++) {
        if (i + CHS_magic_len <= datasz && memcmp(PTRADD(pdata, i), CHS_magic, CHS_magic_len) == 0) {
            result = GAME_LOCALE_CHS;
            break;
        }
        if (i + CHT_magic_len <= datasz && memcmp(PTRADD(pdata, i), CHT_magic, CHT_magic_len) == 0) {
            result = GAME_LOCALE_CHT;
            break;
        }
    }
	
done:
    if (lpMapAddress) UnmapViewOfFile(lpMapAddress);
    if (hMapFile) CloseHandle(hMapFile);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
    
    return result;
}