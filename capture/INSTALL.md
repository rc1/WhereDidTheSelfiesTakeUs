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

    