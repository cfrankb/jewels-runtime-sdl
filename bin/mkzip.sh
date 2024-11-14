#!/bin/bash
mkdir -p build/jewels
rm build/jewels/*
cp build/jewels.* build/jewels
mv build/jewels/jewels.html build/jewels/index.html
cd build/jewels
zip jewels.zip jewels* index.html