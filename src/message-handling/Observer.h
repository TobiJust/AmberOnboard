/*
 * Observer.h
 *
 *  Created on: 31.10.2014
 *      Author: administrator
 */

#ifndef OBSERVER_H_
#define OBSERVER_H_

class Observer {
public:
    Observer();
    virtual ~Observer();
    virtual void update() = 0;
};

#endif /* OBSERVER_H_ */
