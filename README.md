[![Build](https://github.com/caaallum/nsargs/actions/workflows/build.yml/badge.svg)](https://github.com/caaallum/nsargs/actions/workflows/build.yml)
[![Test](https://github.com/caaallum/nsargs/actions/workflows/test.yml/badge.svg)](https://github.com/caaallum/nsargs/actions/workflows/test.yml)

# nsargs — NSIS Command Line Argument Parser

A lightweight NSIS plugin for parsing command line arguments with support for both long (`--verbose`) and short (`-v`) flags, as well as options that accept values (`--output myfile.txt`).

---

## Functions

### `nsargs::AddOption`

Registers a command line option before parsing begins.

**Syntax**
```nsis
nsargs::AddOption "longFlag" "shortFlag" "hasValue"
```

**Parameters**

| Parameter | Type | Description |
|-----------|------|-------------|
| `longFlag` | String | The long form of the flag (used with `--`) |
| `shortFlag` | String | The short form of the flag (used with `-`) |
| `hasValue` | String (`"0"` or `"1"`) | `"1"` if the option expects a value, `"0"` if it is a boolean flag |

**Examples**
```nsis
; Boolean flag — no value expected
nsargs::AddOption "verbose" "v" "0"

; Value option — expects an argument after the flag
nsargs::AddOption "output" "o" "1"
nsargs::AddOption "config" "c" "1"
```

---

### `nsargs::Parse`

Parses the command line arguments. Must be called after all options have been registered with `AddOption`.

**Syntax**
```nsis
nsargs::Parse
Pop $result
```

**Return value (via `Pop`)**

| Value | Meaning |
|-------|---------|
| `"1"` | Parsing succeeded |
| `"0"` | Parsing failed |

**Example**
```nsis
nsargs::Parse
Pop $0
${If} $0 == "0"
    MessageBox MB_OK "Failed to parse command line arguments."
    Abort
${EndIf}
```

---

### `nsargs::GetOption`

Retrieves the value or presence of a previously registered option. You can pass either the long or short flag name.

**Syntax**
```nsis
nsargs::GetOption "flagName"
Pop $result
```

**Parameters**

| Parameter | Type | Description |
|-----------|------|-------------|
| `flagName` | String | Either the long or short flag name (without dashes) |

**Return value (via `Pop`)**

| Value | Meaning |
|-------|---------|
| `"1"` | Boolean flag was present on the command line |
| `"0"` | Flag was not present |
| `"<value>"` | The value provided with the option (for options registered with `hasValue = "1"`) |

**Examples**
```nsis
; Check a boolean flag
nsargs::GetOption "verbose"
Pop $0
${If} $0 == "1"
    DetailPrint "Verbose mode enabled."
${EndIf}

; Retrieve a value option (using short flag name)
nsargs::GetOption "o"
Pop $0
${If} $0 != "0"
    DetailPrint "Output path: $0"
${EndIf}
```

---

## Full Example

The following example registers several options, parses the command line, and then acts on the results.

```nsis
Section "Main"

    ; Register options
    nsargs::AddOption "verbose" "v" "0"
    nsargs::AddOption "output"  "o" "1"
    nsargs::AddOption "config"  "c" "1"

    ; Parse arguments
    nsargs::Parse
    Pop $0
    ${If} $0 == "0"
        MessageBox MB_OK "Argument parsing failed."
        Abort
    ${EndIf}

    ; Check verbose flag
    nsargs::GetOption "verbose"
    Pop $0
    ${If} $0 == "1"
        DetailPrint "Verbose mode is ON."
    ${EndIf}

    ; Get output path
    nsargs::GetOption "output"
    Pop $0
    ${If} $0 != "0"
        DetailPrint "Writing output to: $0"
    ${Else}
        DetailPrint "No output path specified."
    ${EndIf}

    ; Get config path (using short flag)
    nsargs::GetOption "c"
    Pop $0
    ${If} $0 != "0"
        DetailPrint "Using config file: $0"
    ${EndIf}

SectionEnd
```

Given the command line:
```
installer.exe --verbose --output "C:\build\result.txt" -c "settings.ini"
```

This would print:
```
Verbose mode is ON.
Writing output to: C:\build\result.txt
Using config file: settings.ini
```

---

## Notes

- `AddOption` calls must all come **before** `Parse`.
- `GetOption` can be called with either the long or short flag name interchangeably.
- For value options (`hasValue = "1"`), always check that the returned value is not `"0"` before using it, as `"0"` indicates the option was absent.