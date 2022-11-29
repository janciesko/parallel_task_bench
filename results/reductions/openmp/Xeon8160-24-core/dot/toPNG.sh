#!/bin/bash

#Call this from within the app result directory and make sure it contains a PNG dir

COMMAND="python3 $1"
ls -f . | while read -r file; do echo $file; echo $COMMAND; `$COMMAND $file`; done


