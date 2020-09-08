# Exposure Notification Utilities
A little helper tool for Apple's and Google's Exposure Notification (or contact tracing).
Take a look at: https://developer.apple.com/documentation/exposurenotification 

## Compilation

Go into the git directory,

Install mbedtls dependency:
```
git submodule update --init --recursive
```

### Using Dockerfile

```
docker build . -t en-utils
```

### CMake

Build using CMake
```
mkdir build && cd build
```

```
cmake ..
```

```
make
```

## Usage

### Using Docker

You can either build your own image or use the pre-built one from Dockerhub (https://hub.docker.com/r/covidbraceletprj/en-utils).

#### Using Dockerhub
Running without volumes:
```
docker run --rm covidbraceletprj/en-utils:latest generate_identifiers 75c734c6dd1a782de7a965da5eb93125 2642976
```

Running with volume:
```
docker run --rm \
              -v $(pwd):/en-utils \
              covidbraceletprj/en-utils:latest generate_identifiers 75c734c6dd1a782de7a965da5eb93125 2642976 output.txt
```


#### Locally tagged

Running without volumes:
```
docker run --rm en-utils generate_identifiers 75c734c6dd1a782de7a965da5eb93125 2642976
```

Running with volume:
```
docker run --rm \
              -v $(pwd):/en-utils \
              en-utils generate_identifiers 75c734c6dd1a782de7a965da5eb93125 2642976 output.txt
```

### Using local binary
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

### Decrypting Metadata
Example:
```
en_utils decrypt_metadata 75c734c6dd1a782de7a965da5eb93125 2642976 72033874
```

### Generate CSV
Example:
```
en_utils generate_csv 75c734c6dd1a782de7a965da5eb93125 2642976
```

Or APPENDS to file (table head is added when creating a file):
```
en_utils generate_csv 75c734c6dd1a782de7a965da5eb93125 2642976 output.csv
```