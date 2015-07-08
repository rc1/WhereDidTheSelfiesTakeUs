#include "ThreadedExecQueue.h"

void ThreadedExecQueue::threadedFunction () {

    while ( isThreadRunning() ) {
        string commandToDo = "";

        // Check for a command
        lock();
        if ( !commands.empty() ) {
            commandToDo = commands.front();
            commands.pop();
        }
        unlock();

        if ( commandToDo != "" ) {
            printf( "Preforming: %s", commandToDo.c_str() );
            system( commandToDo.c_str() );
        }
        
    }
    
}

void ThreadedExecQueue::addCommand ( string command )  {
    lock();
    commands.push( command );
    unlock();
}

int ThreadedExecQueue::queueSize ()  {
    int size = 0;
    lock();
    size = commands.size();
    unlock();
    return size;
}