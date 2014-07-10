#!/bin/bash
find src/duke/ -name '*.?pp' -exec clang-format -i {} \;
find test/ -name '*.?pp' -exec clang-format -i {} \;
