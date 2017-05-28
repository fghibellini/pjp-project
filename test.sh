#!/usr/bin/env bash

for f in programs/*; do
    if out=$((./bin/milac || exit 1) < $f 2>/dev/null); then
        echo -n [OK]
    else
        echo -n [fail]
    fi
    printf "\t%s\n" "$f"
    #echo "$out"
    #echo
done
