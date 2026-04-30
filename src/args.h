#ifndef __ARGS_H
#define __ARGS_H

#define ARGS_MAX_OPTIONS     64
#define ARGS_MAX_POSITIONALS 64
#define ARGS_MAX_NAME_LEN    64
#define ARGS_MAX_VALUE_LEN   256

typedef struct {
    TCHAR long_name[ARGS_MAX_NAME_LEN];
    TCHAR value[ARGS_MAX_VALUE_LEN];
    BOOL present;
} args_entry_t;

/**********************************************************
 * \brief Parses command-line arguments and stores them in shared memory.
 * 
 * \param argc The number of command-line arguments.
 * \param argv The array of command-line argument strings.
 * 
 * \return TRUE if parsing was successful, FALSE otherwise.
 **********************************************************/
BOOL args_parse(INT32 argc, PTCHAR argv[]);

/**********************************************************
 * \brief Adds a command-line argument definition to the parser.
 * 
 * \param long_name The long name of the argument (e.g., "help").
 * \param short_name The short name of the argument (e.g., 'h').
 * \param has_value Indicates whether the argument expects a value (TRUE or FALSE).
 **********************************************************/
VOID args_add_arg(LPCWSTR long_name, TCHAR short_name, BOOL has_value);

/**********************************************************
 * \brief Resolves a command-line argument by its name and retrieves its value.
 * 
 * \param name The name of the argument to resolve (either long or short).
 * 
 * \return A pointer to an args_entry_t structure containing the argument's value and presence status, or NULL if the argument is not found.
 **********************************************************/
const args_entry_t* args_resolve(LPCWSTR name);

#endif /* __ARGS_H */