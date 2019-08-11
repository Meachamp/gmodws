
# gmod_ws
gmod_ws is a binary that can upload garry's mod GMA files to the workshop. That's right, you don't need to use graphical steam or gmpublish! This is particularly attractive for automated build solutions involving workshop addons. 

This project is now a standalone binary. See `gmodws` in the `compiled/` folder. 

Some other goodies are included in this project:
- Reverse engineered headers for steamCMD's appID based IRemoteStorage API
- Reverse engineered headers for steamCMD's steamAPI utilities
- Great example code for Bootil usage and IRemoteStorage API usage
- Reverse engineered steamclient interface

# Usage
First, you'll need to get yourself a copy of linux steamCMD. Download it from [here.](https://steamcdn-a.akamaihd.net/client/installer/steamcmd_linux.tar.gz) Unpack and run it once. 

Run steamcmd again and login with your chosen account once in order to cache the credentials. gmod_ws will later use these credentials to login. 
```
/path/to/steamcmd.sh +login account_name +quit
```

Unpack all the files in the `compiled/` folder. You're free to move them wherever, just be aware that the `gmodws` depends on `steamclient.so`. The binary can be run as below.

```
./gmodws account_name 123456 /path/to/addon.gma
```

Optionally, you can specify a fourth argument to the command. This will all you to control the message that appears in the update changelogs on steam. 
```
./gmodws account_name 123456 /path/to/addon.gma "My test commit message"
```

# Compiling

If compiling bootil, follow these steps. 

- Download bootil from [here.](https://github.com/garrynewman/bootil)

```
cd build/bootil/projects
premake5 gmake
cd build/bootil/projects/linux/gmake
make all
```

If compiling the binary itself:
```
cd build/gmodws
make
```
- Source `steamclient.so` from the `compiled/` folder, or an install of steamCMD. Be aware that it is the 64 bit version. 
