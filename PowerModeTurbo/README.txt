
This is Power Mode Turbo, also known as Code in the Light. As someone
mesmerized by bright colors and flashy displays, I was impressed by
the code editor for Code in the Dark, and the humorous extension for Atom
called "Power Mode". However, I felt they were just scratching the
surface, and wanted to make an extension for that. Power Mode Turbo,
inspired by both of those, seeks to be as flashy as possible while
editing text files. Note: As there are flashing lights, this may be
a danger to epileptics.

Author: Maximillian McMullen, who wrote it during MHacks 8.

Dependencies: The project utilizes the free and open source library
openFrameworks. As there is no platform specific code, this project
should work on Linux and Mac, but I have only tested it on Windows.

Compile Instructions: Open up the Visual Studio project, and compile
the project using Visual C++. The two fonts, consolas and player2, must
be moved to the data folder after compilation in order for the program
to function correctly.

Usage: Just start up PowerModeTurbo.exe. There is an optional command
line parameter to specify a file to edit (in binary mode). If a file was
specified, changes will be saved to that file on exit. If no file was
specified, all text will be written to "default.txt".
