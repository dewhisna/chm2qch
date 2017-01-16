# chm2qch #

Tool for converting Windows CHM files to Qt Help format.
Based on [chmlib](http://www.jedrea.com/chmlib/) and some 
[kchmviewer](http://www.ulduzsoft.com/linux/kchmviewer/) sources.

[GPL](https://www.gnu.org/licenses/gpl-3.0.en.html).

### Installation ###

* Qt 5.2 or higher recommended.
* chmlib 0.40 (included)

```
qmake chm2qch.pro
make
```

For Qt <= 5.2 (4.8, etc.) use **chm2qch_qt4.pro**, which includes experimental QCommandLineParser support for Qt 4. To build chm2qch with GUI support use **chm2qch_gui.pro**.

### Running ###

```
chm2qch [options] helpfile.chm
```

chm2qch will create helpfile.qhp and extract all HTML files from CHM to current directory. 

| Option                 | Description                                     |
|------------------------|-------------------------------------------------|
| -?, -h, --help         | Displays help.                                  |
| -g, --generate         | Run qhelpgenerator to produce QCH file          |
| -c, --clean            | Delete intermediate files (use with -g)         |
| -r, --no-root          | Do not write root contents section              |
| -q, --quiet            | Quiet mode. Do not write any messages to stdout |
| -d, --directory <dir>  | Set target directory to <dir>                   |
| -n, --namespace <name> | Set documentation namespace to <name>           |