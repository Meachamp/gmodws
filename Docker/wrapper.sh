#!/bin/bash
ARGUMENT=${1,,}

if [ "$ARGUMENT" == "login" ]; then
    ~/Steam/steamcmd.sh +login $STEAM_USER +quit
    exit 0
elif [ "$ARGUMENT" == "upload" ]; then
    cd ~/gmodws
    ./gmodws $STEAM_USER $2 ~/upload/$3 "${@:4}" 
else
    echo "[ERROR] No argument supplied"
    exit 1
fi
