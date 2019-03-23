# gmod_ws
gmod_ws is a module that interfaces with steamCMD to upload garry's mod GMA files to the workshop. That's right, you don't need to use graphical steam or gmpublish! With this module you can update straight from steamCMD. This is particularly attractive for automated build solutions involving workshop addons. 

This module adds a single command to steamCMD: gmod_update_ws. This command takes two parameters: the workshop id and the file to upload (this should be absolute). 

Some other goodies are included in this project:
- Reverse engineered headers for steamCMD's appID based IRemoteStorage API
- Reverse engineered headers for steamCMD's steamAPI utilities
- Great example code for Bootil usage and IRemoteStorage API usage
- Dynamically linked bootil project

# Usage
First, you'll need to get yourself a copy of linux steamCMD. Download it from [here.](https://steamcdn-a.akamaihd.net/client/installer/steamcmd_linux.tar.gz) Unpack, run it once, then move all the files to a folder named steamcmd. Drop compiled/preload.so and compiled/libbootil.so into the linux32 folder. Drop the launcher/steamcmd.sh folder outside of the steamcmd folder. 

Next, you should run steamcmd and login with your chosen account once, in order to cache the credentials. After that, you can automatically run a workshop update with something like this:
```
/path/to/custom/steamcmd.sh +login account_name +gmod_update_ws 123456 /path/to/addon.gma +quit
```

Optionally, you can specify a third argument to the command. This will all you to control the message that appears in the update changelogs on steam. 
```
gmod_update_ws 123456 /path/to/addon.gma "My test commit message"
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

If compiling the module itself, follow these steps.

- Run PATH=/path/to/valve/runtime/bins:$PATH
- Change directories to src
- Get a copy of libbootil and put it somewhere. You can use the compiled version included with this project, or compile it yourself with the instructions above. 
- Run the following command:
```
/path/to/valve/steam-runtime/bin/g++ -std=c++0x -shared -fPIC -m32 -I path/to/bootil/include -L/path/to/custom/libs -lbootil preload.c -o preload.so
```
- Put libbootil and preload shared objects in the linux32 of steamcmd