
# gmod_ws
gmod_ws is a binary that can upload garry's mod GMA files to the workshop. That's right, you don't need to use graphical steam or gmpublish! This is particularly attractive for automated build solutions involving workshop addons. 

This project is now a standalone binary. See `gmodws` in the `compiled/` folder. 

Some other goodies are included in this project:
- Reverse engineered headers for steamCMD's appID based IRemoteStorage API
- Reverse engineered headers for steamCMD's steamAPI utilities
- Great example code for Bootil usage and IRemoteStorage API usage
- Dynamically linked bootil project
- Reverse engineered steamclient interface

# Usage
First, you'll need to get yourself a copy of linux steamCMD. Download it from [here.](https://steamcdn-a.akamaihd.net/client/installer/steamcmd_linux.tar.gz) Unpack and run it once. 

Run steamcmd again and login with your chosen account once in order to cache the credentials. gmod_ws will later use these credentials to login. 
```
/path/to/steamcmd.sh +login account_name +quit
```

Unpack all the files in the `compiled/` folder. You're free to move them wherever, just be aware that the `gmodws` depends on all the shared objects in that folder. The binary can be run as below.

```
./gmodws account_name 123456 /path/to/addon.gma
```

Optionally, you can specify a fourth argument to the command. This will all you to control the message that appears in the update changelogs on steam. 
```
./gmodws account_name 123456 /path/to/addon.gma "My test commit message"
```

# Compiling
Compiling is a little tricky. Due to limitations involving the C Runtime, you will need to compile using a similar version of the runtime distributed with steamCMD. You can get a toolchain with this runtime [here](https://developer.valvesoftware.com/wiki/Source_SDK_2013#Source_SDK_2013_on_Linux). You must compile with this toolchain. 

If compiling bootil, follow these steps. 

- Download bootil from [here.](https://github.com/garrynewman/bootil)
- Replace premake5_bootil.lua with the file in premake_fixes. 
- Run PATH=/path/to/valve/runtime/bins:$PATH
- Run premake5 gmake
- Configure for release
- Run gmake and grab the artifacts from release_x32

If compiling the binary itself, follow these steps.

- Run PATH=/path/to/valve/runtime/bins:$PATH
- Change directories to src
- Get a copy of libbootil and put it somewhere. You can use the compiled version included with this project, or compile it yourself with the instructions above. 
- Run the following command:
```
/path/to/valve/steam-runtime/bin/g++ -g -std=c++0x -Wl,-rpath,. -m32 -I bootil/include -L/path/to/custom/libs -lm -lpthread -ldl -lbootil main.c -o gmodws
```
- Source the dynamic dependencies from the `compiled/` folder, or an install of steamCMD.
