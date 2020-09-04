# Exposure Notification Utilities
A little helper tool for Apple's and Google's Exposure Notification (or contact tracing).
Take a look at: https://developer.apple.com/documentation/exposurenotification 

## Compilation

Install mbedtls dependency:
```
git submodule update --init --recursive
```
Then build using CMake
```
mkdir build && cd build
```

```
cmake ..
```

```
make
```

## Example Usage

```
en_utils generate_identifiers <tek-hex> <tek-interval> <output_file (optional)>
```

Export to stdout:
```
en_utils generate_identifiers 75c734c6dd1a782de7a965da5eb93125 2642976
```

Or APPENDS to file:
```
en_utils generate_identifiers 75c734c6dd1a782de7a965da5eb93125 2642976 output.txt
```
