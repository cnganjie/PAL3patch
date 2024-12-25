#include "common.h"

char *unicode_to_chinese(const wchar_t *s, const wchar_t *table)
{
    char *buf = malloc(wcslen(s) * 2 + 1);
    char *p = buf;
    while (*s) {
        unsigned short w = *s++;
        if (w < 0x80) {
            *p++ = w;
        } else {
            wchar_t *c = wmemchr(table, w, 0x8000); // very slow!!!
            if (c) {
                int i = (c - table) + 0x8000;
                *p++ = i >> 8;
                *p++ = i;
            } else {
                *p++ = '?';
            }
        }
    }
    *p = 0;
    return buf;
}

wchar_t *chinese_to_unicode(const char *s, const wchar_t *table)
{
    wchar_t *buf = malloc(sizeof(wchar_t) * (strlen(s) + 1));
    wchar_t *p = buf;
    while (*s) {
        wchar_t c = 0xfffd;
        unsigned char b1 = *s++;
        if (b1 < 0x80) {
            c = b1;
        } else if (*s) {
            unsigned char b2 = *s++;
            int i = ((b1 << 8) | b2) - 0x8000;
            if (table[i]) c = table[i];
        }
        *p++ = c;
    }
    *p = 0;
    return buf;
}

char *utf16_to_utf8(const wchar_t *s)
{
    char *buf = malloc(wcslen(s) * 3 + 1);
    char *p = buf;
    while (*s) {
        unsigned c = 0xfffd;
        unsigned short w1 = *s++;
        if ((w1 & 0xf800) != 0xd800) {
            c = w1;
        } else if ((w1 & 0xfc00) == 0xd800) {
            unsigned short w2 = *s;
            if ((w2 & 0xfc00) == 0xdc00) {
                c = (((w1 & 0x3ff) << 10) | (w2 & 0x3ff)) + 0x10000;
                s++;
            }
        }
        if (c < (1 << 7)) {
            *p++ = c;
        } else if (c < (1 << 11)) {
            *p++ = 0xc0 | (c >> 6);
            *p++ = 0x80 | (c & 0x3f);
        } else if (c < (1 << 16)) {
            *p++ = 0xe0 | (c >> 12);
            *p++ = 0x80 | ((c >> 6) & 0x3f);
            *p++ = 0x80 | (c & 0x3f);
        } else {
            *p++ = 0xf0 | (c >> 18);
            *p++ = 0x80 | ((c >> 12) & 0x3f);
            *p++ = 0x80 | ((c >> 6) & 0x3f);
            *p++ = 0x80 | (c & 0x3f);
        }
    }
    *p = 0;
    return buf;
}

wchar_t *utf8_to_utf16(const char *s)
{
    wchar_t *buf = malloc(sizeof(wchar_t) * (strlen(s) + 1));
    wchar_t *p = buf;
    while (*s) {
        int i, n;
        unsigned c;
        unsigned char b = *s++;
        if (b <= 0x7f) {
            n = 0; c = b;
        } else if (b < 0xc2) {
            n = 0; c = 0xfffd;
        } else if (b <= 0xdf) {
            n = 1; c = b & 0x1f;
        } else if (b <= 0xef) {
            n = 2; c = b & 0xf;
        } else if (b <= 0xf4) {
            n = 3; c = b & 0x7;
        } else {
            n = 0; c = 0xfffd;
        }
        for (i = 1; i <= n; i++) {
            int l = 0x80, h = 0xbf;
            if (i == 1) {
                switch (b) {
                case 0xe0: l = 0xa0; break;
                case 0xed: h = 0x9f; break;
                case 0xf0: l = 0x90; break;
                case 0xf4: h = 0x8f; break;
                }
            }
            b = *s;
            if (b < l || b > h) {
                c = 0xfffd; break;
            }
            c = (c << 6) | (b & 0x3f);
            s++;
        }
        if (c < 0x10000) {
            *p++ = c;
        } else {
            c -= 0x10000;
            *p++ = 0xd800 | (c >> 10);
            *p++ = 0xdc00 | (c & 0x3ff);
        }
    }
    *p = 0;
    return buf;
}

#define SAFE_CS2WCS L"cs2wcs() failed."
#define SAFE_WCS2CS "wcs2cs() failed."
#define NO_GLOBAL_MANAGED_FUNCTIONS

// convert a mbcs-string to an unicode-string with given codepage
// will alloc memory, don't forget to free()
wchar_t *cs2wcs_alloc(const char *cstr, UINT src_cp)
{
    if (src_cp == CP_UTF8) {
        return utf8_to_utf16(cstr);
    }
    
    wchar_t *ret = NULL;
    size_t len;
    
    // get string length first
    len = MultiByteToWideChar(src_cp, 0, cstr, -1, NULL, 0);
    if (len == 0) goto fail;
    
    // alloc buffer
    ret = (wchar_t *) malloc(sizeof(wchar_t) * len);
    if (!ret) goto fail;
    
    // do real convert
    if (MultiByteToWideChar(src_cp, 0, cstr, -1, ret, len) == 0) goto fail;
    return ret;

fail:
    free(ret);
#ifdef SAFE_CS2WCS
    return wcsdup(SAFE_CS2WCS);
#else
    return NULL;
#endif
}

// convert an unicode-string to a mbcs-string with given codepage
// will alloc memory, don't forget to free()
char *wcs2cs_alloc(const wchar_t *wstr, UINT dst_cp)
{
    if (dst_cp == CP_UTF8) {
        return utf16_to_utf8(wstr);
    }
    
    char *ret = NULL;
    size_t len;
    
    // get string length first
    len = WideCharToMultiByte(dst_cp, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (len == 0) goto fail;
    
    // alloc buffer
    ret = (char *) malloc(len);
    if (!ret) goto fail;
    
    // do real convert
    if (WideCharToMultiByte(dst_cp, 0, wstr, -1, ret, len, NULL, NULL) == 0) goto fail;
    return ret;
    
fail:
    free(ret);
#ifdef SAFE_WCS2CS
    return strdup(SAFE_WCS2CS);
#else
    return NULL;
#endif
}


// convert a mbcs-string in src_cp to a mbcs-string in dst_cp
// will alloc memory, don't forget to free()
char *cs2cs_alloc(const char *cstr, UINT src_cp, UINT dst_cp)
{
    wchar_t *wstr;
    char *ret = NULL;
    
    // convert to unicode-string first
    wstr = cs2wcs_alloc(cstr, src_cp);
    if (!wstr) goto done;
    
    // convert back to mbcs-string
    ret = wcs2cs_alloc(wstr, dst_cp);
    
done:
    free(wstr);
    return ret;
}



// return pointer to managed memory
wchar_t *cs2wcs_managed(const char *cstr, UINT src_cp, wchar_t **pptr)
{
    free(*pptr);
    return *pptr = cs2wcs_alloc(cstr, src_cp);
}
char *wcs2cs_managed(const wchar_t *wstr, UINT dst_cp, char **pptr)
{
    free(*pptr);
    return *pptr = wcs2cs_alloc(wstr, dst_cp);
}
char *cs2cs_managed(const char *cstr, UINT src_cp, UINT dst_cp, char **pptr)
{
    free(*pptr);
    return *pptr = cs2cs_alloc(cstr, src_cp, dst_cp);
}


#ifndef NO_GLOBAL_MANAGED_FUNCTIONS

// return pointer to globally managed memory (vaild until next call)
wchar_t *cs2wcs(const char *cstr, UINT src_cp)
{
    static wchar_t *ptr = NULL;
    return cs2wcs_managed(cstr, src_cp, &ptr);
}
char *wcs2cs(const wchar_t *wstr, UINT dst_cp)
{
    static char *ptr = NULL;
    return wcs2cs_managed(wstr, dst_cp, &ptr);
}
char *cs2cs(const char *cstr, UINT src_cp, UINT dst_cp)
{
    static char *ptr = NULL;
    return cs2cs_managed(cstr, src_cp, dst_cp, &ptr);
}

#endif
