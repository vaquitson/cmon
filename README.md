# Cmon


## License
This project is licensed under the [MIT License](LICENSE).

Cmon is a tool that automatically restarts a command whenever specified files are modified.
It's especially useful for restarting a development server while working on a web application or any project that benefits from hot-reloading.

Currently, Cmon only works on Linux, but we are working to make it available on Windows and macOS.# Installation.

## Requirements (Linux)

To install Cmon, make sure you have the following dependencies:

- **Cmake**
- **pstree**
- **glibc**
- **llhttp**

### Steps

Clone the repository and build the binary with make:

```sh
git clone https://github.com/vaquitson/cmon.git
cd cmon

mkdir build
cd build
cmake ..

make
```
# Usage
Cmon executes a single command that you provide, and automatically restarts it when matching files are changed.

For example:
```sh
cmon go run main
```
This will run **go run main* and restart it when changes are detected.

## Configuration
Cmon looks for a configuration file named .config.cmon in the current working directory (CWD).
If it doesn't find one, it will create it with default content like this:

```
WATCH_FILE_EXT_NAMES: {
  "html",
  "css",
  }
  
IGNORE_FILES: {
    ".gitignore",
    "go.mod"
    "go.sum"
  }
  
IGNORE_DIRS: {
    ".git",
    "build",
    "node_modules",
    "templates/test",
  }
```
⚠️ Note: For now, files and directories must be specified without the ./ prefix. That syntax is not currently supported.

### Available Configuration Fields
* WATCH_FILE_EXT_NAMES:
    A list of file extensions that Cmon watches for changes (e.g., "html", "go"). When changes are detected, the command is restarted.

* IGNORE_FILES: 
    A list of specific files to ignore when monitoring for changes.

* WATCH_FILE_EXT_NAMES: this tell wich extension names to ignore.
