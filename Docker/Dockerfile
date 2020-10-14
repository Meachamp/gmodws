FROM ubuntu:18.04
LABEL maintainer="Aperture Development <webmaster@Aperture-Development.de>"
ENV DEBIAN_FRONTEND noninteractive
# Prepare Container Enviroment
RUN useradd -ms /bin/bash gmodws &&\
    dpkg --add-architecture i386 &&\
    apt-get update &&\
    apt-get -y --no-install-recommends install software-properties-common lib32gcc1 curl lib32stdc++6 ca-certificates libsdl2-2.0-0:i386 unzip &&\
    add-apt-repository ppa:ubuntu-toolchain-r/test &&\
    apt-get update &&\
    apt-get -y install --reinstall lib32stdc++6 libstdc++6 &&\
    apt-get clean autoclean &&\
    apt-get autoremove -y &&\
    rm -rf /var/lib/apt/lists/*
# Copy shell script into container and make them runable
COPY downloadLatest.sh /home/gmodws/downloadLatest.sh
COPY wrapper.sh /wrapper.sh
RUN chmod +x /home/gmodws/downloadLatest.sh &&\
    chmod +x /wrapper.sh
# Switch user and working directory
USER gmodws
WORKDIR /home/gmodws
# Prepare folders and download SteamCMD
RUN mkdir ~/upload ~/gmodws ~/Steam &&\ 
    cd ~/Steam &&\
    curl -sqL "https://steamcdn-a.akamaihd.net/client/installer/steamcmd_linux.tar.gz" | tar zxvf -
# Download the latest gmodws release, unzip it and make it runable
RUN bash ~/downloadLatest.sh ~/gmodws/gmodws.zip &&\
    cd ~/gmodws &&\
    unzip gmodws.zip &&\
    chmod +x gmodws steamclient.so
# Set wrapper as entrypoint
ENTRYPOINT ["/wrapper.sh"]