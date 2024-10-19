# Usage Guide

## Basic Usage

Compress a file:
./gzip-clone -c file.txt

Decompress a file:
./gzip-clone -d file.txt.gz

## Options

- -c, --compress: Compress the specified files (default action)
- -d, --decompress: Decompress the specified .gz files
- -l, --level [1-9]: Set compression level (1 = fastest, 9 = best compression)
- -o, --output [FILE/DIR]: Specify output file or directory
- -r, --recursive: Recursively compress/decompress files in directories
- -t, --threads [NUM]: Set the number of threads to use
- -h, --help: Display help message and exit
- -v, --version: Display version information and exit

## Examples

Compress multiple files:
./gzip-clone -c file1.txt file2.txt file3.txt

Compress a directory recursively:
./gzip-clone -c -r /path/to/directory

Decompress files with 4 threads:
./gzip-clone -d -t 4 file1.gz file2.gz file3.gz
