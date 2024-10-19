# GZIP-Clone

GZIP-Clone is a command-line utility for file compression and decompression, inspired by the original gzip tool. It provides multi-threaded compression and decompression capabilities, recursive directory processing, and supports various compression levels.

## Features

- File compression and decompression using zlib
- Multi-threaded processing for improved performance
- Recursive directory compression and decompression
- Customizable compression levels (1-9)
- Progress reporting during compression/decompression
- Cross-platform compatibility (Unix-like systems and Windows)
- Preservation of file attributes (permissions, timestamps)

## Building

To build GZIP-Clone, you need:

- A C compiler (gcc or clang)
- Make
- zlib library

Clone the repository and build the project:

git clone https://github.com/kaitakami/gzip-clone.git
cd gzip-clone
make

## Usage

Compress a file:
```
./gzip-clone -c file.txt
```

Decompress a file:
```
./gzip-clone -d file.txt.gz
```

For more options, run:
```
./gzip-clone --help
```

## Testing

To run the test suite:

```
make test
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.