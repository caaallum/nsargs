!include "LogicLib.nsh"

Section
    ; Define options
    nsargs::AddOption "output" "o" "1"   ; has value
    nsargs::AddOption "verbose" "v" "0"  ; flag
    nsargs::AddOption "help" "h" "0"     ; flag

    ; Parse (reads GetCommandLine internally, no args needed)
    nsargs::Parse
    Pop $0
    ${If} $0 != "1"
        MessageBox MB_OK "Failed to parse command line"
        Abort
    ${EndIf}

    ; Retrieve values
    nsargs::GetOption "output"
    Pop $R0   ; e.g. "C:\install\path"
    DetailPrint "Output path: $R0"

    nsargs::GetOption "v"
    Pop $R1   ; "1" if --verbose/-v was passed, "" otherwise
    DetailPrint "Verbose: $R1"
SectionEnd