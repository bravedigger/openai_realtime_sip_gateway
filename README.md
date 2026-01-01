# SIP Gateway Connecting OpenAI
a C++ implementation for a SIP gateway bridging SIP calls to OpenAI Realtime Speech API

## Environment
Ubuntu Linux 24.02
g++

## Packages
```
$sudo apt-get install libboost-all-dev libssl-dev nlohmann-json3-dev libpjproject-dev
```

### libpjproject-dev
If missing libpjproject-dev, we will need to install it manually:
```
$sudo apt update
$sudo apt install build-essential git pkg-config cmake python3-minimal libssl-dev libasound2-dev libopus-dev libgsm1-dev libavcodec-dev libavdevice-dev libavformat-dev libavutil-dev libavfilter-dev libswscale-dev libx264-dev libsamplerate-dev libsrtp2-dev
$git clone https://github.com/pjsip/pjproject
$cd pjproject
$./configure --enable-shared
$make dep
$make
$sudo make install
$sudo ldconfig
```

## Compile and Link
```
$ g++ -ggdb openai_realtime.cpp -o openai_realtime $(pkg-config --cflags --libs libpjproject) -lssl -lcrypto -lpthread -lboost_system
```

## Run
Set the environment variable OPENAI_API_KEY first. You will need to generate it from OpenAI developer website.
```
$export OPENAI_API_KEY="sk-..."
$ ./openai_realtime
```

## Troubleshoot
run ldconfig as root if it can't find the libpjsua2.so.2 file:
```
$ ./openai_realtime
./openai_realtime: error while loading shared libraries: libpjsua2.so.2: cannot open shared object file: No such file or directory
$sudo ldconfig
```

