# gmod_ws
gmodws is a binary that can upload garry's mod GMA files to the workshop. That's right, you don't need to use graphical steam or gmpublish! This is particularly attractive for automated build solutions involving workshop addons. 

This project is now a standalone binary. You can download it it from the `Releases` section of this repository. 

# Usage
First, you'll need to get yourself a copy of linux steamCMD. Download it from [here.](https://steamcdn-a.akamaihd.net/client/installer/steamcmd_linux.tar.gz) Unpack and run it once. 

Run steamcmd again and login with your chosen account once in order to cache the credentials. gmod_ws will later use these credentials to login. 
```
/path/to/steamcmd.sh +login account_name +quit
```

Unpack all the files in the `release zip`. You're free to move them wherever, just be aware that the `gmodws` depends on `steamclient.so`. The binary can be run as below.

```
./gmodws account_name 123456 /path/to/addon.gma
```

Optionally, you can specify a fourth argument to the command. This will all you to control the message that appears in the update changelogs on steam. 
```
./gmodws account_name 123456 /path/to/addon.gma "My test commit message"
```

# Without SteamCMD
For certain CI configurations where a new environment is setup each time a build is triggered, SteamCMD may be too much trouble. Instead, you can log in to your steam account without cached credentials and without steamCMD by setting the `STEAM_PASSWORD` environment variable. Please note that this requires steamguard to be disabled. If you go this route, you should have a dedicated account for pushing workshop builds. 

# Using Docker
[Docker](https://docs.docker.com/get-docker/) is a great solution for distributing software, mainly because the enviroment required to run the software gets shipped with it. That means all dependencies are already present and you only need to run the software. Preparing the GModWS container is fairly easy, just run the following commands:
```
docker volume create gmodws_steam
docker run --rm -it -e STEAM_USER=<STEAM LOGIN NAME> -v gmodws_steam:/home/gmodws/Steam aperturedevelopment/gmodws:latest login
```

Now you can easily update your addon using this command:
```
docker run --rm -it -e STEAM_USER=<STEAM LOGIN NAME> -v gmodws_steam:/home/gmodws/Steam -v <DIRECTORY_OF_GMA>:/home/gmodws/upload aperturedevelopment/gmodws:latest upload <Workshop id> <GMA Filename> <Changelog>
```

If the login cache expired you will see a error message saying ``[ERROR] Not logged in or cached data expired, please login`` to fix that you only have to login again.

# Debugging
Setting the `GMODWS_DEBUG` environment variable will cause gmodws to enter verbose mode, which may be helpful for debugging. 

# Compiling

Compiling is as easy as running `make` on a linux machine with `g++`. The only dependencies are `pthread` and `dl`, which should be included with a standard linux install. At runtime, `gmodws` will need a copy of `steamclient.so`. 

- Source `steamclient.so` from the `release zip`, or an install of steamCMD. Be aware that it is the 64 bit version, and the interfaces that `steamclient.so` exports may change without warning in new releases of steamCMD. You should normally stick with the binaries included in the release. 
