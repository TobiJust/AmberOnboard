/** \brief      Entry point of the application.
 *
 * \details     Starts the application by registering signal handlers and running initialization process.
 * \author      Daniel Wagenknecht, Felix Hahn, Askar Massalimov
 * \version     2015-01-12
 */

#include "Initializer.h"

#include <iostream>
#include <stdexcept>
#include <signal.h>

using namespace std;

bool terminating = false;

void sig_handler(int signo)
{
    if(!terminating) {
        cerr << "Received signal " << signo << "( " << strsignal(signo) <<" )" << endl;
        cerr << "Calling terminate!" << endl;

        shared_ptr<M2M_TermBroad> broadcast(new M2M_TermBroad);
        MsgHub::getInstance()->appendMsg(broadcast);
        terminating=true;
    }
}

void sig_pipe(int signo) {
    cerr << "Received signal " << signo << "( " << strsignal(signo) <<" )" << endl;
}

int main() {

    // Register signal handlers for proper system signal handling (kill).
    signal(SIGINT, sig_handler);
    signal(SIGABRT, sig_handler);
    signal(SIGILL, sig_handler);
    signal(SIGSEGV, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGPIPE, sig_pipe);

    try {

        // Initializer instance for managing module startup.
        Initializer init;

        // Return value of initialization.
        uint8_t status;

        // Try initializing modules.
        if (!(status=init.setup())) {

            // Print out status indication message.
            cerr << "------------------- setup done --------------------" <<endl;

            // Spawn threads.
            init.spawn();

        } else  // An error occurred.
            cerr << "setup failed with error " << (uint16_t)status << endl;

    }catch ( const std::exception& e ) {
        cerr << "033[1;31mException\033[0m: " << e.what() << endl;
    }

    return 0;
}
