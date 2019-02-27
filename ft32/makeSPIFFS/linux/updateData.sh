#!/bin/bash

clear;

CODYPATH='/home/vmuser/workspace/atom-ide/cody-pp/dist/';
ESPDATAPATH='/home/vmuser/workspace/arduino-ide/ft32/makeSPIFFS/linux/data';

echo "Moving files from $CODYPATH to $ESPDATAPATH ..."
echo

rm -rf $ESPDATAPATH/*
cp -r $CODYPATH* $ESPDATAPATH

rm $ESPDATAPATH/arrow.svg
rm $ESPDATAPATH/pause.svg
rm $ESPDATAPATH/play.svg
rm $ESPDATAPATH/stop.svg
rm $ESPDATAPATH/vendor.js
rm $ESPDATAPATH/main.js

