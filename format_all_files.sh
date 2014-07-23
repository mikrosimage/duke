#!/bin/bash
git status --porcelain --untracked-files=all| grep -e "\.[hc]pp$" | cut -c4- | xargs clang-format -i
