# pseudocode-compiler

A utility and CGI program which translates AQA's GCSE specification pseudocode (course code 8252) to Javascript.

## Building under Windows

Prerequisites:

* Visual Studio 2019 (or 2022) with C++ compiler supporting `/std:c++20`

* Both `win_flex.exe` and `win_bison.exe` from https://github.com/lexxmark/winflexbison

Method:

* Unzip the contents of the release zipfile to a new directory and place all of `win_flex.exe`, `FlexLexer.h` `win_bison.exe` and `data` into the same directory

* Open a Visual Studio command prompt and navigate to this directory

* Run `nmake /f Makefile.nmake` and locate generated `psc.exe`

## Building under Linux

Prerequisites:

* GNU C++ compiler (`g++`) supporting `-std=c++20`

* Both `flex` and `bison`, tested with flex 2.6.4 and bison 3.8.1

Method:

* Unzip the contents of the release zipfile to a user directory and navigate to it

* Run `make` to generate `psc`

## Running the compiler

Simply run `psc.exe` or `./psc` in a console/command prompt window to read from the keyboard and output to the console, redirections using `<` and `>` work equally well too. (Note: to end input under Linux use `Ctrl-D`, under Windows `Ctrl-Z` followed by `Enter`.) See the example programs in the `progs` directory.

Alternatively, specify an input filename, and optionally an output filename, to avoid the need for redirections:

```
./psc myprog.psc            # outputs to myprog.js
./psc input.psc output.js   # outputs to output.js
```

The Javascript output is compatible with the `jsNN` executable from Mozilla (supplied by the Debian packages `libmozjs-NN-dev`), and with NodeJS (see option `-n` below). This output is not meant to be interesting but should always be correct, in case of an input (syntax) error, no output is generated.

Input options (immediately after `psc` and before any redirections or filenames):

* `-h` (or `/?` under Windows) outputs a help text and quits

* `-v` (or `/v` under Windows) outputs the version number and quits

* `-n` (or `/n` under Windows) prepends a small support library to the output program, which supports NodeJS. Note: you may need to run `npm install readline-sync` (once) to allow support for NodeJS.

## Use as a CGI program

The supplied `pseudocode.html` web-page together with `styles.css` and `base64.js` can be used as a front-end to the compiler utility. Under Linux/Apache these files would need to be copied to `/var/www/html/`, or under Windows to `C:\inetpub\wwwroot\`.

Permissions would need to be given to the utility to run as a CGI program, under Linux/Apache it would need to be copied to `/usr/lib/cgi-bin/`, or under Windows to the same directory as before.

The minimum setup under Linux is:

```
$ sudo apt install apache2  # command for Debian/Ubuntu
$ sudo a2enmod cgi
```

possibly with `sudo a2ensite ...` and restarting the `apache2` service too. See your distro's documentation.

Windows needs Internet Information Services (IIS) Manager to be enabled and set up together with a permissions rule for the executable binary, see documentation for your version of Windows (tested under Windows 10). Note that **some editing of `pseudocode.html` is necessary**: `/cgi-bin/psc?` needs to be changed to `./psc.exe?` (note the `.`), assuming that the utility is located in the same directory as the web-page.

Note that inputs to the running program have to be entered into the text field bottom-left **before** the program is run, being separated by commas. This is the only difference to running the Javascript in a console window.

## License

This software is released under the highly permissive MIT License. If you do modify or improve parts of this software, especially the web front-end, I hope you would want to share your efforts, but this is not a requirement.

For this software to be useful to school students, setting up the web interface would be of great advantage, as the edit/compile/run cycle using just the console has an associated learning curve.

Please note that this software is **not** either endorsed or supported by AQA Exam Board, and also its author is not affiliated to them in any way.

## Resources

Pseudocode Specification as an unrestricted download from the AQA website:

https://filestore.aqa.org.uk/resources/computing/AQA-8525-NG-PC.PDF

Note that the only change from the spec (as far as I am aware) is for `SUBROUTINE` parameters needing to be followed by `: Type` as for `RECORD` fields (but not when the `SUBROUTINE` is called).