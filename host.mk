# this file is not used by the build system but is intended as documentation
# on some assumed environment 'variables' as featured on the authors host OS.

TEST := C:\MinGW\msys\1.0\bin\[.exe
FIND := "C:\Program Files (x86)\GnuWin32\bin\find.exe"
DOT  := "C:\Program Files (x86)\Graphviz2.38\bin\dot.exe"

PATH=
  C:\Program Files (x86)\Microchip\mplabc30\v3.31\bin;
  C:\Program Files (x86)\Microchip\xc16\v1.23\bin;
  C:\Program Files (x86)\GnuWin32\bin;
  C:\Program Files\TortoiseSVN\bin;

  C:\Ruby193\bin;
  C:\HashiCorp\Vagrant\bin;
  C:\Qt\Qt5.2.1\5.2.1\msvc2010\bin;
  C:\Program Files (x86)\GNU Tools ARM Embedded\4.7 2014q2\bin;


C:\>make.exe
GNU Make 3.81
This program built for i386-pc-mingw32

C:\>python --version
Python 2.7.6 --  64-bit

C:\>[.exe --version
[ (GNU coreutils) 5.3.0
Written by Kevin Braunsdorf and Matthew Bradburn.

C:\>find.exe /?
Searches for a text string in a file or files.

FIND [/V] [/C] [/N] [/I] [/OFF[LINE]] "string" [[drive:][path]filename[ ...]]

  /V         Displays all lines NOT containing the specified string.
  /C         Displays only the count of lines containing the string.
  /N         Displays line numbers with the displayed lines.
  /I         Ignores the case of characters when searching for the string.
  /OFF[LINE] Do not skip files with offline attribute set.
  "string"   Specifies the text string to find.
  [drive:][path]filename
             Specifies a file or files to search.

If a path is not specified, FIND searches the text typed at the prompt
or piped from another command.
