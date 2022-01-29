#!/bin/bash

gcc -o todoer main.c

echo Do you want to add 'todoer' to path yes/no?

read INPUT

if [ $INPUT == "yes" ]; then
    sudo cp todoer /usr/local/bin
    echo Added to path successfully.
else
    echo Ok, Exiting.
fi
