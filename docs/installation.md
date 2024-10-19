# Installation Guide

## Prerequisites

- C Compiler (gcc or clang)
- Make
- zlib library
- Check unit testing framework (for running tests)

## Steps

1. Clone the repository:
   ```
   git clone https://github.com/kaitakami/gzip-clone.git
   cd gzip-clone
   ```

2. Build the project:
   ```
   make
   ```

3. (Optional) Install the Check framework for running tests:
   - On Ubuntu/Debian:
     ```
     sudo apt-get install check
     ```
   - On macOS (using Homebrew):
     ```
     brew install check
     ```

4. (Optional) Build and run tests:
   ```
   make test
   ```

The `gzip-clone` executable will be created in the project root directory.
