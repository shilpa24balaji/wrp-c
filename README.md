# wrp-c

C implementation of the Web Routing Protocol

[![Build Status](https://travis-ci.org/Comcast/wrp-c.svg?branch=master)](https://travis-ci.org/Comcast/wrp-c)
[![codecov.io](http://codecov.io/github/Comcast/wrp-c/coverage.svg?branch=master)](http://codecov.io/github/Comcast/wrp-c?branch=master)
[![Apache V2 License](http://img.shields.io/badge/license-Apache%20V2-blue.svg)](https://github.com/Comcast/wrp-c/blob/master/LICENSE.txt)

# Building and Testing Instructions

```
mkdir build
cd build
cmake ..
make
make test
make coverage
firefox index.html
```

# Coding Formatter Settings

Please format pull requests using the following command to keep the style consistent.

```
astyle -A10 -S -f -U -p -D -c -xC90 -xL
```
