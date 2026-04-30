#ifndef __UTIL_H
#define __UTIL_H

/**********************************************************
 * \brief Convert TCHAR to wchar_t
 *
 * \param src TCHAR to convert
 * \param dst Destination tchar_t
 * \param max Max length
 **********************************************************/
VOID tchar_to_wchar(_In_ LPCTSTR src, _Out_ LPWSTR dst, _In_ INT max);

#endif /* __UTIL_H */