#!/bin/bash

mkdir ~/.hsh

make

sudo cp hsh /usr/local/bin

cd ~/.hsh

touch history

touch user

touch password

touch selfdef

echo "hsh is set up, you can now remove the hsh directory"