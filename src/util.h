#ifndef __UTIL_H
#define __UTIL_H

/**********************************************************
 * \brief Convert TCHAR to utf8 char
 *
 * \param src TCHAR to convert
 * \param dst Destination char*
 * \param max Max length
 **********************************************************/
VOID tchar_to_utf8(_In_ const TCHAR* src, _Out_ LPSTR dst, _In_ INT max);

/**********************************************************
 * \brief Convert TCHAR to wchar_t
 *
 * \param src TCHAR to convert
 * \param dst Destination tchar_t
 * \param max Max length
 **********************************************************/
VOID tchar_to_wchar(_In_ LPCTSTR src, _Out_ LPWSTR dst, _In_ INT max);

#endif /* __UTIL_H */