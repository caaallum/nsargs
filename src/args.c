#include <Windows.h>
#include "args.h"

#define SHARED_MEM_NAME TEXT("Local\\NSARGS")

typedef struct {
    TCHAR long_name[ARGS_MAX_NAME_LEN];
    TCHAR short_name;
    BOOL has_value;
} args_option_def_t;

typedef struct {
    args_option_def_t defs[ARGS_MAX_OPTIONS];
    INT32 def_count;
    args_entry_t entries[ARGS_MAX_OPTIONS];
    INT32 entry_count;
    TCHAR positionals[ARGS_MAX_POSITIONALS][ARGS_MAX_VALUE_LEN];
    INT32 positional_count;
} args_t;

//==========================================================
static args_t*
get_args(VOID) {
    HANDLE hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_MEM_NAME);

    if (!hMap) {
        hMap = CreateFileMapping(
            INVALID_HANDLE_VALUE, NULL,
            PAGE_READWRITE, 0, sizeof(args_t),
            SHARED_MEM_NAME
        );
    }

    if (!hMap) {
        return NULL;
    }

    // Intentionally leak hMap — keeps mapping alive across DLL unloads
    return (args_t*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(args_t));
}


//==========================================================
static const args_option_def_t*
args_find_long_def(LPCWSTR name) {
    args_t* args = get_args();
    for (int i = 0; i < args->def_count; i++) {
        if (lstrcmp(args->defs[i].long_name, name) == 0) {
            return &args->defs[i];
        }
    }
    return NULL;
}

//==========================================================
static const args_option_def_t*
args_find_short_def(TCHAR c) {
    args_t* args = get_args();
    for (int i = 0; i < args->def_count; i++) {
        if (args->defs[i].short_name == c) {
            return &args->defs[i];
        }
    }
    return NULL;
}

//==========================================================
static args_entry_t*
args_get_or_create_entry(LPCWSTR long_name) {
    args_t* args = get_args();
    for (int i = 0; i < args->entry_count; i++) {
        if (lstrcmp(args->entries[i].long_name, long_name) == 0) {
            return &args->entries[i];
        }
    }
    if (args->entry_count >= ARGS_MAX_OPTIONS) {
        return NULL;
    }
    args_entry_t* e = &args->entries[args->entry_count++];
    memset(e, 0, sizeof(*e));
    lstrcpyn(e->long_name, long_name, ARGS_MAX_NAME_LEN);
    return e;
}

//==========================================================
static void
args_store(LPCWSTR long_name, LPCWSTR value) {
    args_entry_t* e = args_get_or_create_entry(long_name);
    if (!e) {
        return;
    }
    e->present = TRUE;
    if (value) {
        lstrcpyn(e->value, value, ARGS_MAX_VALUE_LEN);
    }
}

//==========================================================
const args_entry_t*
args_resolve(LPCWSTR name) {
    args_t* args = get_args();
    /* single char = short name */
    if (name[0] != '\0' && name[1] == '\0') {
        const args_option_def_t* def = args_find_short_def(name[0]);
        if (def) {
            for (int i = 0; i < args->entry_count; i++) {
                if (lstrcmp(args->entries[i].long_name, def->long_name) == 0) {
                    return &args->entries[i];
                }
            }
        }
        return NULL;
    }
    for (int i = 0; i < args->entry_count; i++) {
        if (lstrcmp(args->entries[i].long_name, name) == 0) {
            return &args->entries[i];
        }
    }
    return NULL;
}

//==========================================================
BOOL
args_parse(INT32 argc, PTCHAR argv[]) {
    args_t* args = get_args();
    if (!args) {
        return FALSE;
    }

    args->entry_count = 0;
    args->positional_count = 0;

    for (int i = 1; i < argc; i++) {
        const TCHAR* arg = argv[i];

        if (wcsncmp(arg, TEXT("--"), 2) == 0) {
            const TCHAR* name = arg + 2;
            const args_option_def_t* def = args_find_long_def(name);
            if (!def) return -1;

            if (def->has_value) {
                if (i + 1 >= argc) return -1;
                args_store(name, argv[++i]);
            }
            else {
                args_store(name, NULL);
            }

        }
        else if (arg[0] == '-' && arg[1] != '\0') {
            for (int j = 1; arg[j] != '\0'; j++) {
                TCHAR c = arg[j];
                const args_option_def_t* def = args_find_short_def(c);
                if (!def) return -1;

                if (def->has_value) {
                    const TCHAR* value;
                    if (arg[j + 1] != '\0') {
                        value = arg + j + 1;
                        j += (int)lstrlen(value);
                    }
                    else if (i + 1 < argc) {
                        value = argv[++i];
                    }
                    else {
                        return -1;
                    }
                    args_store(def->long_name, value);
                    break;
                }
                else {
                    args_store(def->long_name, NULL);
                }
            }

        }
        else {
            if (args->positional_count >= ARGS_MAX_POSITIONALS) return -1;
            lstrcpyn(args->positionals[args->positional_count++], arg, ARGS_MAX_VALUE_LEN);
        }
    }
    return TRUE;
}

//==========================================================
VOID
args_add_arg(LPCWSTR long_name, TCHAR short_name, BOOL has_value) {
    args_t* args = get_args();
    if (!args || args->def_count >= ARGS_MAX_OPTIONS) {
        return;
    }
    args_option_def_t* def = &args->defs[args->def_count++];
    lstrcpyn(def->long_name, long_name, ARGS_MAX_NAME_LEN);
    def->short_name = short_name;
    def->has_value = has_value;
}