#!/bin/bash

MAIN=../../../../..

ASL=iasl

START=$1
LEN=$2
NAME=$(dd bs=4 if=$MAIN/edOS.fs count=1 status=none skip=$(python3 -c "print(128 * $START)"))  # gets the signature of the ACPI table

mkdir $NAME

dd if=$MAIN/edOS.fs of=$NAME/$NAME.aml skip=$START count=$LEN

$ASL -d $NAME/$NAME.aml