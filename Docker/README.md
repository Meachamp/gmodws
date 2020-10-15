# Docker Instructions

## Manually building the container
First ``cd`` into the Docker directory of this repository and then run this command:
```
docker build . -t gmodws:latest
```

The building process will take some time, but in the end you can just use ``gmodws:latest`` as your ``<IMAGE>`` variable in the usage instructions

## Using the hosted container
Aperture Development provides a hosted GModWS container on the DockerHub at this repository: https://hub.docker.com/repository/docker/aperturedevelopment/gmodws
That container will get constantly updated, and to use it you only have to use ``aperturedevelopment/gmodws:latest`` as your ``<IMAGE>`` variable in the usage instructions.


## How to use the docker container
```
docker volume create gmodws_steam
docker run --rm -it -e STEAM_USER=<STEAM LOGIN NAME> -v gmodws_steam:/home/gmodws/Steam <IMAGE> login
```

Now you can easily update your addon using this command:
```
docker run --rm -it -e STEAM_USER=<STEAM LOGIN NAME> -v gmodws_steam:/home/gmodws/Steam -v <DIRECTORY_OF_GMA>:/home/gmodws/upload <IMAGE> upload <Workshop id> <GMA Filename> <Changelog>
```

If the login cache expired you will see a error message saying ``[ERROR] Not logged in or cached data expired, please login`` to fix that you only have to login again.
