#pragma once

#include "ofMain.h"

class ThreadedExecQueue : public ofThread {
 public:

    void threadedFunction ();

    void addCommand ( string command );
    
    int queueSize ();

 protected:
    queue<string> commands;
};
