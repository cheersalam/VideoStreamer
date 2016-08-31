# VideoStreamer

A fun project to receive video stream from **Parrot Drone** and render it on screen without using any Parrot SDK. This project can also be used for creating mpegts files. it uses ffmpeg for decoding and SDL for rendering.

**TODO:**
- 

1. ~~Create HLS stream from incoming stream~~
2. Add Audio
3. Create RTSP stream from incoming stream
4. Create RTMP stream from incoming stream
 
# Dependencies
 * `cmake`: for compiling project
 * `yasm` needed by ffmpeg
 * `libsdl-dev` for rendering  
 * `lighttpd` for web server (HLS streaming)

  `sudo apt-get install cmake yasm libsdl-dev lighttpd`


# Compiling project
 Go to root of the project

 * create debug directory
 * Enter debug directory
 * generate makefiles using cmake
 * Compile whole project
 
```Linux
$ mkdir /var/www/html/parrot/  (for HLS streaming)
$ sudo chown pi:pi /var/www/html/parrot/  (on Pi, for other *nix use your machine user)
$ cd VideoStreamer
$ mkdir debug
$ cd debug
$ cmake ..
$ make
```
> Note: If compiling on pi3, it takes around 1 hour to compile whole project. Once ffmpeg is compiled it takes less than a minute to compile whole project again.


# Running Application
(Connect to Bebop WiFi)
```Linux
$ cd VideoStreamer/debug/bin
$ export LD_LIBRARY_PATH=../lib
$ ./BebopVideoStream
``` 

> Note: By default Drone Ip address is set as 192.168.42.1 and command port is set as 4444

To run application for different Ip address and port 
```Linux
$ ./BebopVideoStream -i 192.168.43.1 -p 55555
``` 

# For HLS streaming
```http://<pi/*nix ip address>/parrot/parrot.m3u8```

> Note: machine running software and on PC you want to receive HLS streaming should be on same network

There are two ways you can achieve that
On your pi connect wired network to your local LAN and use wifi to connect to Bebop, then on machine you want to receive HLS connect to same local LAN and use pi ip adress for HLS

On machine you want to receive HLS connect to Drone WiFi and use pi ip address for recieving HLS streaming.

# Verified on
- **Parrot Bebop2 on Ubuntu/Fedora and Raspberry Pi3**


# Bugs, Improvements or feature addition
mailto: cheersalam@gmail.com

# Setup

![alt tag](https://github.com/cheersalam/VideoStreamer/blob/master/test/Delay_measurement.JPG)

![alt tag](https://github.com/cheersalam/VideoStreamer/blob/master/test/Setup.JPG)

# License

LGPL
