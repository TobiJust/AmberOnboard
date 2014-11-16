//============================================================================
// Name        : MessageHub.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "message-handling/MsgHub.h"
#include "Module.h"
#include "ModuleIO.h"
#include "ModuleConfiguration.h"


#include "io-handling/Terminal.h"
#include "io-handling/IOHandler.h"
#include "io-handling/StreamIO.h"

#include <iostream>
#include <unistd.h>

using namespace std;

int main() {

    ModuleConfiguration* confMod = new ModuleConfiguration();
    ModuleIO* ioModule = new ModuleIO();

    thread* modIOThread = new thread(&ModuleIO::run, ioModule);
    thread* modConfThread = new thread(&ModuleConfiguration::run, confMod);

    modConfThread->join();
    modIOThread->join();

    /*
    // MessageHub::getInstance();

    // New instance, using stdin and stdout as stream.
    StreamIO s_io(cin, cout);

    // Polymorph interface pointer to StreamIO.
    IOHandler *s_ioH = &s_io;

    Terminal *term = new Terminal(s_ioH);
    thread termThread = term->createThread();

    termThread.join();
     */

    return 0;
}
