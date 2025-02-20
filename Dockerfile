FROM debian:bookworm
RUN apt update
RUN apt-get install -y build-essential xorg-dev libxss-dev libxxf86vm-dev \
    libxkbfile-dev libxv-dev libxrandr-dev libxinerama-dev libxcursor-dev \
    libxi-dev libcurl4-gnutls-dev  libgtk-3-dev libglib2.0-dev \
    libwebkit2gtk-4.1-dev libasound2-dev wget libssl-dev
RUN wget https://github.com/Kitware/CMake/releases/download/v3.27.1/cmake-3.27.1.tar.gz
RUN tar -zxvf cmake-3.27.1.tar.gz
RUN cd cmake-3.27.1 && ./bootstrap && make && make install
RUN apt-get install -y python3-pip
RUN pip3 install --break-system-packages conan==2.11.0
RUN conan profile detect
CMD /source/build/build_release.sh
