#include "ThreadedRunCommand.h"

void ThreadedRunCommand::threadedFunction () {

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
            system( commandToDo.c_str() );
        }
        
    }
    
}

void ThreadedRunCommand::addCommand ( string command )  {
    lock();
    commands.push( command );
    unlock();
}
