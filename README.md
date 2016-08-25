# VideoStreamer

A fun project to receive video stream from **Parrot Drone** and render it on screen without using any Parrot SDK. This project can also be used for creating mpegts files. it uses ffmpeg for decoding and SDL for rendering.

**VideoContainerGenerator:** This project depends on another project called VideoContainerGenerator (https://*github.com/cheersalam/VideoContainerGenerator.git*) for rendering and mpegts container generation. You don't need to install this project. VideoStreamer will automatically compile and install VideoContainerGenerator. It compiles ffmpeg in **LGPL v1.2** version and uses ffmpeg's inbuilt h264 decoder.



**TODO:**
- 

1. Create HLS stream from incoming stream
2. Add Audio
3. Create RTSP stream from incoming stream
4. Create RTMP stream from incoming stream
 
# Dependencies
 * `cmake`: for compiling project
 * `yasm` needed by ffmpeg
 * `libsdl-dev` for rendering  

  `sudo apt-get install cmake yasm libsdl-dev`


# Compiling project
 Go to root of the project

 * create debug directory
 * Enter debug directory
 * generate makefiles using cmake
 * Compile whole project
 
```Linux
$ cd VideoContainerGenerator
$ mkdir debug
$ cd debug
$ cmake ..
$ make
```
> Note: If compiling on pi3, it takes around 1 hour to compile whole project. Once ffmpeg is compiled it takes less than a minute to compile whole project again.


# Running Application
```Linux
$ cd VideoContainerGenerator/debug/bin
$ export LD_LIBRARY_PATH=../lib
$ ./BebopVideoStream
``` 

> Note: By default Drone Ip address is set as 192.168.42.1 and command port is set as 4444

To run application for different Ip address and port 
```Linux
$ ./BebopVideoStream -i 192.168.43.1 -p 55555
``` 

# Verified on
- **Parrot Bebop2 on Ubuntu/Fedora and Raspberry Pi3**


# Bugs, Improvements or feature addition
mailto: cheersalam@gmail.com

# License
