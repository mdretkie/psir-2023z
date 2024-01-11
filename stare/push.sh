#!/usr/bin/env bash
set -e
git add "$(dirname ${BASH_SOURCE[0]})"
git status
git commit -m "Zmiany"
git push
