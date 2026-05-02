#include <stddef.h>   // for size_t
#include <stdarg.h>   // for va_list
#include <setjmp.h>   // for setjmp/longjmp
#define CMOCKA_STATIC 1
#include <cmocka.h>

#include <windows.h>
#include <stdio.h>
#include "../nsis/pluginapi.h"
#include "../src/util.h"

#define TEST_STRING_SIZE 1024

static TCHAR g_vars_storage[__INST_LAST * TEST_STRING_SIZE];

// Implement the three globals that pluginapi.c references via extern
unsigned int  g_stringsize = TEST_STRING_SIZE;
stack_t* g_stacktop_storage = NULL;
stack_t** g_stacktop = &g_stacktop_storage;
TCHAR* g_variables = g_vars_storage;
LPTSTR g_testing_command_line = TEXT("nsargs_test.exe -o output -v");

static void
dump_stack(void) {
    stack_t* p = *g_stacktop;
    int i = 0;
    _tprintf(_T("  -- stack (top first) --\n"));
    while (p) {
        _tprintf(_T("  [%d] \"%s\"\n"), i++, p->text);
        p = p->next;
    }
    if (i == 0)
        _tprintf(_T("  (empty)\n"));
}

static void
dump_vars(void) {
    static const TCHAR* names[] = {
        _T("$0"),  _T("$1"),  _T("$2"),  _T("$3"),  _T("$4"),
        _T("$5"),  _T("$6"),  _T("$7"),  _T("$8"),  _T("$9"),
        _T("$R0"), _T("$R1"), _T("$R2"), _T("$R3"), _T("$R4"),
        _T("$R5"), _T("$R6"), _T("$R7"), _T("$R8"), _T("$R9"),
        _T("$CMDLINE"), _T("$INSTDIR"), _T("$OUTDIR"),
        _T("$EXEDIR"), _T("$LANGUAGE")
    };
    _tprintf(_T("  -- variables --\n"));
    for (int i = 0; i < __INST_LAST; i++) {
        const TCHAR* val = getuservariable(i);
        if (val && val[0] != _T('\0'))
            _tprintf(_T("  %-12s = \"%s\"\n"), names[i], val);
    }
}

static void
clear_stack(void) {
    while (*g_stacktop) {
        stack_t* top = *g_stacktop;
        *g_stacktop = top->next;
        GlobalFree((HGLOBAL)top);
    }
}

/** Zero all NSIS variables. */
static void
clear_vars(void) {
    memset(g_vars_storage, 0, sizeof(g_vars_storage));
}

/** Reset everything between tests. */
static void
reset(void) {
    clear_stack();
    clear_vars();
}

// -----------------------------------------------------------------------
// Forward-declare nscj functions
// -----------------------------------------------------------------------

// Example – replace with the real names from nscj.c
extern void __cdecl AddOption(HWND hwndParent, int string_size,
    TCHAR* variables, stack_t** stacktop, void* extra);
extern void __cdecl Parse(HWND hwndParent, int string_size,
    TCHAR* variables, stack_t** stacktop, void* extra);
extern void __cdecl GetOption(HWND hwndParent, int string_size,
    TCHAR* variables, stack_t** stacktop, void* extra);

// -----------------------------------------------------------------------
// Convenience macro – calls a plugin function using the harness globals
// -----------------------------------------------------------------------
#define CALL_PLUGIN(fn) \
    fn(NULL, TEST_STRING_SIZE, g_variables, g_stacktop, NULL)

// -----------------------------------------------------------------------
// String comparison wrapper for cmocka
// -----------------------------------------------------------------------
#define assert_tchar_equal(expected, actual)           \
    do {                                              \
        CHAR exp_utf8[TEST_STRING_SIZE];                \
        CHAR act_utf8[TEST_STRING_SIZE];                \
        tchar_to_utf8(expected, exp_utf8, TEST_STRING_SIZE); \
        tchar_to_utf8(actual, act_utf8, TEST_STRING_SIZE);   \
        assert_string_equal(exp_utf8, act_utf8);     \
    } while(0)

static void
test_args_parse(void** state) {
    TCHAR result[TEST_STRING_SIZE];
    reset();

    pushstring(_T("1"));
    pushstring(_T("o"));
    pushstring(_T("output"));
    CALL_PLUGIN(AddOption);

    pushstring(_T("0"));
    pushstring(_T("h"));
    pushstring(_T("help"));
    CALL_PLUGIN(AddOption);

    pushstring(_T("0"));
    pushstring(_T("v"));
    pushstring(_T("verbose"));
    CALL_PLUGIN(AddOption);

    CALL_PLUGIN(Parse);
    popstring(result);

    pushstring(_T("output"));
    CALL_PLUGIN(GetOption);
    popstring(result);
    assert_tchar_equal(result, TEXT("output"));

    pushstring(_T("help"));
    CALL_PLUGIN(GetOption);
    popstring(result);
    assert_tchar_equal(result, TEXT("0"));

    pushstring(_T("verbose"));
    CALL_PLUGIN(GetOption);
    popstring(result);
    assert_tchar_equal(result, TEXT("1"));
}

int
main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_args_parse),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}