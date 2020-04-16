#pragma once
#include <wchar.h>
static int ttmp;
#define errno_t int
//extern errno_t _wfopen_s(FILE **fp, wchar_t const* _FileName, wchar_t const* _Mode);
#define _wfopen_s(fp, fn, md) ((*fp = _wfopen(fn, md)), (ttmp=errno), fprintf(stderr, "Ex: %d\n", ttmp), ttmp)
// errno_t _wfopen_s(FILE **fp, wchar_t const* _FileName, wchar_t const* _Mode) {
//     *fp = _wfopen(_FileName, _Mode);
//     return 0;
// }