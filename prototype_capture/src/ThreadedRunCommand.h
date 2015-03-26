#pragma once

#include "ofMain.h"

class ThreadedRunCommand : public ofThread {
 public:

    void threadedFunction ();

    void addCommand ( string command );

 protected:
    queue<string> commands;
};
