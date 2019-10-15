/*
 * Author : pufan
 * Created on : 2019-10-09
 * Contract : 517993945@qq.com
 */

#ifndef COROUTINE_H
#define COROUTINE_H

#include <deque>

#include <boost/context/continuation.hpp>

#include "epoll.h"

class CoFrame;

class Routine {

public:

    virtual void process();

    Routine();

private:

    void _realRun();

    boost::context::continuation _cc;

    boost::context::continuation _oc;

    bool _start;

    friend class CoFrame;
};

class CoDaemon: public Routine {

public:

    virtual void process();

};


class CoFrame {

public:

    static CoFrame* ins();

    bool init(Routine* daemon);

    Routine* getCurrentRoutine();

    void setCurrentRoutine(Routine* r);

    Epoll* getPoller();

    void schedule();

    void addRunnable(Routine* r);

private:

    Routine* _daemon;
    Routine* _current;

    Epoll  _poller;

    std::deque<Routine*> _runnables;
};

#endif
