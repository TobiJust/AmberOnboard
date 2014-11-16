/** \brief      Message Hub for inter-module communication.
 *
 * \details     Class for realizing encapsulated communication of the application modules.
 *              Implements Observer pattern for module notification.
 *              Implements Singleton pattern for global uniqueness.
 * \author      Daniel Wagenknecht
 * \version     2014-10-31
 * \class       MsgHub
 */


#ifndef MESSAGEHUB_H_
#define MESSAGEHUB_H_


#include "Observer.h"
#include "msg/Msg.h"

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <queue>

using namespace std;

class MsgHub {
public:

    virtual ~MsgHub();
    static MsgHub* getInstance();

    void attachObserverToMsg(Observer* observer, int type);
    void detachObserverFromMsg(Observer* observer, int type);
    void notifyObservers(int type);

    Msg* getMsg(Observer*);
    int getMsgCount(Observer*);
    void appendMsg(Msg* message);

private:

    MsgHub();
    static MsgHub* instance;    // Singleton instance.

    // Message observer section.
    unordered_map<int, unordered_set<Observer*>> map_MsgType_ObsList;   // List 1
    unordered_map<Observer*, queue<Msg*>* > map_Obs_MsgList;            // List 2
    unordered_map<Msg*, int> map_Msg_ObsCount;                          // List 3

    // Mutex section.
    unordered_map<Observer*, mutex* > map_Obs_Mutex;                    // List 4
    mutex mutex_MsgType_ObsList;    // Mutex 1
    mutex mutex_Obs_MsgList;        // Mutex 2
    mutex mutex_Msg_ObsCount;       // Mutex 3
    mutex mutex_Obs_Mutex;          // Mutex 4
};

#endif /* MESSAGEHUB_H_ */
