#ifndef PAL3PATCH_MISC_H
#define PAL3PATCH_MISC_H


#define ERROR_FILE "PAL3patch.error.txt"
#define WARNING_FILE "PAL3patch.log.txt"

extern int is_win9x();
#define fail(fmt, ...) __fail(__FILE__, __LINE__, __func__, fmt, ## __VA_ARGS__)
extern void __attribute__((noreturn)) __fail(const char *file, int line, const char *func, const char *fmt, ...);
#define warning(fmt, ...) __plog(1, __FILE__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define plog(fmt, ...) __plog(0, __FILE__, __LINE__, __func__, fmt, ## __VA_ARGS__)
extern void __plog(int is_warning, const char *file, int line, const char *func, const char *fmt, ...);
extern int str2int(const char *valstr);
extern double str2double(const char *valstr);

#endif