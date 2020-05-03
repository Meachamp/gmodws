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

# Compiling

Compiling is as easy as running `make` on a linux machine with `g++`. The only dependencies are `pthread` and `dl`, which should be included with a standard linux install. At runtime, `gmodws` will need a copy of `steamclient.so`. 

- Source `steamclient.so` from the `release zip`, or an install of steamCMD. Be aware that it is the 64 bit version, and the interfaces that `steamclient.so` exports may change without warning in new releases of steamCMD. You should normally stick with the binaries included in the release. 
