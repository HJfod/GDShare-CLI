# GDShare CLI

This is a command-line version of GDShare, meant for users who have experienced problems with the GUI version.

## Installation

1. Go to the [latest release](https://github.com/HJfod/GDShare-CLI/releases/latest)
2. Download the app (`GDShareCLI.zip`)
3. Extract into a folder of choice
4. Open a command line (Press Windows Key and search "gmd")
5. Navigate to the folder using the `cd` command (Picture below)
6. Type `gdshare.exe` (If you are on a 32-bit machine, type `gdshare32.exe`) (If you are using Powershell, type `./gdshare.exe`)

![image](https://user-images.githubusercontent.com/60038575/108225292-b4788480-7133-11eb-9e02-aa3a5bf8a4d5.png)

## Basic Usage

### Exporting levels

```
./gdshare.exe <type> <level name> <level name> <level name> ...
```

 * `<level-name>` is the name of the level. **If the level name contains spaces, wrap the name in quotes** `"Example name"`. The name is not case-sensitive.
 * `<type>` is either `as-gmd`, `as-gmd2` or `as-lvl`. These are the supported **Export Formats**. Default is `.gmd`.

### Import levels

```
./gdshare.exe <path/to/file> <path/to/file2> ...
```

 * `<path-to-file>` is the path to the level file you want to import. **If the path contains spaces, wrap the pah in quotes** `"C:/Example path/file.gmd"`. Supported formats are `.gmd`, `.gmd2` and `.lvl`.

### Find level name

```
./gdshare.exe find <search term>
```

 * `<search term>` is the string used to search for the level. It is not case-sensitive, ignores whitespace and checks if the level name contains the string **anywhere in it**.

### List levels

```
./gdshare.exe list
```