#!/bin/sh
set -eu
source_file=""
output_file=""
while [ "$#" -gt 0 ]; do
    case "$1" in
        -f) source_file="$2"; shift 2 ;;
        -o) output_file="$2"; shift 2 ;;
        *) shift ;;
    esac
done
test -n "$source_file"
test -n "$output_file"
mkdir -p "$(dirname "$output_file")"
cp "$source_file" "$output_file"
