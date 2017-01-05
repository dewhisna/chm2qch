# chm2qch #

Tool for converting Windows CHM files to Qt Help format.
Based on [chmlib](http://www.jedrea.com/chmlib/) and some 
[kchmviewer](http://www.ulduzsoft.com/linux/kchmviewer/) sources.

[GPL](https://www.gnu.org/licenses/gpl-3.0.en.html).

### Installation ###

* Qt 5.2 or higher required.
* chmlib 0.40 (included)

```
qmake
make
```

### Running ###

```
chm2qch [options] helpfile.chm
```

chm2qch will create helpfile.qhp and extract all HTML files from CHM to current directory. 

| Option                 | Description                                     |
|------------------------|-------------------------------------------------|
| -?, -h, --help         | Displays help.                                  |
| -g, --generate         | Run qhelpgenerator to produce QCH file          |
| -r, --no-root          | Do not write root contents section              |
| -q, --quiet            | Quiet mode. Do not write any messages to stdout |
| -n, --namespace <name> | Set documentation namespace to <name>           |