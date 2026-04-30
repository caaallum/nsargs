#include <windows.h>
#include <nsis/pluginapi.h>

#include "args.h"
#include "util.h"

HINSTANCE g_hInstance;
HWND g_hwndParent;

#define NSISFUNC(_name) void __declspec(dllexport) _name( \
    HWND hwnd_parent, int string_size, TCHAR *variables,  \
    stack_t **stacktop, extra_parameters *extra)

NSISFUNC(AddOption) {
    EXDLL_INIT();

    TCHAR long_name[ARGS_MAX_NAME_LEN] = { 0 };
    TCHAR short_name_str[4] = { 0 };
    TCHAR has_value_str[4] = { 0 };

    popstring(long_name);
    popstring(short_name_str);
    popstring(has_value_str);

    args_add_arg(long_name, short_name_str[0], _tcsicmp(has_value_str, _T("1")) == 0);
}

NSISFUNC(Parse) {
    EXDLL_INIT();

    INT32    argc = 0;
    WCHAR command_line_w[ARGS_MAX_VALUE_LEN];
    
    PTCHAR command_line = GetCommandLine();
    tchar_to_wchar(command_line, command_line_w, lstrlen(command_line) + 1);

    LPWSTR* argv = CommandLineToArgvW(command_line_w, &argc);
    if (!argv) {
        pushstring(TEXT("0"));
        return;
    }

    int result = args_parse(argc, argv);

    GlobalFree(argv);

    pushstring(result ? TEXT("1") : TEXT("0"));
}

NSISFUNC(GetOption) {
    EXDLL_INIT();

    TCHAR name[ARGS_MAX_NAME_LEN] = { 0 };
    popstring(name);

    const args_entry_t* e = args_resolve(name);

    if (!e || !e->present) {
        pushstring(TEXT(""));
        return;
    }

    if (e->value[0] == '\0') {
        pushstring(TEXT("1"));
    }
    else {
        pushstring(e->value);
    }
}

BOOL WINAPI DllMain(HINSTANCE hInst, ULONG ul_reason_for_call, LPVOID lpReserved) {
    g_hInstance = hInst;
    return TRUE;
}
