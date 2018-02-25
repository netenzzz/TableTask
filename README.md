# TableTask
Not implemented features:
1.Threads processing is not implemented


Build instructions(On MacOS and Linux)

1.Install gtk+2.0/3.0(https://www.gtk.org/download/) and gtkextra-3.0(http://gtkextra.sourceforge.net) .
Or just open terminal and write(brew install gtkextra) and then (brew install gtk+).
I think you can do same installations on Linux systems.

2. Open MakeFile.am .
Write instead of "/usr/local/bin/g++-7" you compiler path(it should support c++11).
Build programm using command "make -f Makefile.am" from directory where your downloaded project files . 