#include <Windows.h>

#include "util.h"

//==========================================================
VOID
tchar_to_wchar(_In_ LPCTSTR src, _Out_ LPWSTR dst, _In_ INT max) {
#ifdef UNICODE
    (void)lstrcpyn(dst, src, max);
#else
    MultiByteToWideChar(CP_ACP, 0, src, -1, dst, max);
#endif
}
