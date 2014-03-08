#!/bin/bash
find src/duke/ -name '*.?pp' -exec clang-format -i {} \;
