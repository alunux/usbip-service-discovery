#!/bin/bash

clang-format -fallback-style=Mozilla -i $(find . -name '*.[ch]' -not -path '*/src/libs/*')
