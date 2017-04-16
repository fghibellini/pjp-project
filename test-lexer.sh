#!/usr/bin/env bash

for i in lexer-tests/*.in; do 
	res="$(bin/parse --lex < "$i")" 
    if [[ $? -ne 0 ]]; then
        echo "PARSE_ERROR"
	elif diff $(echo "$i" | sed 's/\.in$/.out/') <(echo "${res}"); then 
		echo "OK" 
	else 
		echo "MISMATCH" 
	fi 
done 
