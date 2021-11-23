#!/bin/bash
ARGUMENT=${1,,}

if [ "$ARGUMENT" == "login" ]; then
    ~/Steam/steamcmd.sh +login $STEAM_USER +quit
    exit 0
elif [ "$ARGUMENT" == "upload" ]; then
    ~/Steam/steamcmd.sh +login $STEAM_USER +quit > /home/gmodws/steamcmd.log&
    sleep 5s
    LOGGED_IN=$(grep -o 'to Steam Public...OK' /home/gmodws/steamcmd.log)

    if [ "$LOGGED_IN" == "to Steam Public...OK" ]; then
        cd ~/gmodws
        ./gmodws $STEAM_USER $2 ~/upload/$3 "${@:4}" 
        exit 0
    else
        echo "[ERROR] Not logged in or cached data expired, please login"
        exit 1
    fi
else
    echo "[ERROR] No argument supplied"
    exit 1
fi
