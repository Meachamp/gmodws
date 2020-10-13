#!/bin/bash
GMODWS_LATEST=$(curl --silent "https://api.github.com/repos/Meachamp/gmodws/releases/latest" | grep -Po '"tag_name": "\K.*?(?=")')
GMODWS_ZIP=$(echo "https://github.com/Meachamp/gmodws/releases/download/$GMODWS_LATEST/gmodws.zip")
curl -sqL $GMODWS_ZIP -o $1
exit 0