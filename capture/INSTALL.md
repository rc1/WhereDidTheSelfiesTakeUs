# Installation instructions 
## OSX

This uses gstreamer-1.0 and plugins. They need to be installed with options in order to work. If you have already installed gst-launch-1.0 there is a good chance it won't work.

To install BASE:

    brew uninstall --force gst-plugins-base
    brew install gst-plugins-base --with-theora --with-libvorbis

To install GOOD:

    brew uninstall --force gst-plugins-good
    brew install gst-plugins-good --with-libpng

Note:  
Options can be found by typing: `brew info [formula]`

## Video (Animation Overlay)

This needs to be called "Overlay.mov" and placed in the capture apps data folder.


## Linux

    apt-get install gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-plugins-base gstreamer1.0-plugins-bad gstreamer1.0-omx gstreamer1.0-tools

Wiring Pi is also needed. See [this](http://openframeworks.cc/setup/raspberrypi/Raspberry-Pi-Using-the-GPIO-pins-with-Wiring-Pi-and-openFrameworks.html)

Or:

```Shell
git clone git://git.drogon.net/wiringPi
cd wiringPi
./build
```

    
