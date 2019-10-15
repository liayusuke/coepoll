/*
 * Author : pufan
 * Created on : 2019-10-09
 * Contract : 517993945@qq.com
 */

#include "coroutine.h"
#include "util.h"


void dummy() {};

Routine::Routine() {
    _start = false;
}

void Routine::process() {

}

void Routine::_realRun() {
    namespace ctx = boost::context;
    _start = true;
    _cc = ctx::callcc([this](ctx::continuation&& c) -> ctx::continuation {
            this->_oc = std::move(c);
            this->process();
            return std::move(this->_oc);
            });

    dummy();

    LOG_INFO("_realRun return %p", this);
}

void CoDaemon::process() {

    for (;;) {

        if (!CoFrame::ins()->getPoller()->run(3000)) {
            LOG_ERRO("poller run error");
            continue;
        }

        std::vector<Fly> flys = CoFrame::ins()->getPoller()->getRunnable();
        LOG_INFO("flys num %d", (int)flys.size());

        for (Fly& f : flys) {
            CoFrame::ins()->addRunnable((Routine*)f.ptr);
        }

        CoFrame::ins()->schedule();
    }
}

CoFrame* CoFrame::ins() {
    static CoFrame* _ins = NULL;
    if (!_ins) {
        _ins = new CoFrame();
    }
    return _ins;
}

bool CoFrame::init(Routine* daemon) {

    if (!_poller.init()) {
        return false;
    }

    _daemon = daemon;
    _current = NULL;

    return true;
}

Epoll* CoFrame::getPoller() {

    return &_poller;
}

Routine* CoFrame::getCurrentRoutine() {
    return _current;
}

void CoFrame::setCurrentRoutine(Routine* r) {
    _current = r;
}

void CoFrame::schedule() {

    static int counter = 0;
    LOG_DBUG("counter %d", ++counter);

    Routine* cur = getCurrentRoutine();

    if (cur) {
        LOG_INFO("%s %p switch oc", typeid(*cur).name(), cur);
        setCurrentRoutine(NULL);
        cur->_oc = cur->_oc.resume();
        return;
    }

    if (!_runnables.empty()) {
        cur = _runnables.front();
        _runnables.pop_front();
        LOG_INFO("switch to runnable %s %p", typeid(*cur).name(), cur);
    } else {
        cur = _daemon;
        if (CoFrame::getCurrentRoutine() == _daemon) {
            return;
        }
        LOG_INFO("switch to daemon");
    }

    CoFrame::setCurrentRoutine(cur);

    if (cur->_start) {
        LOG_INFO("resume %s %p", typeid(*cur).name(), cur);
        cur->_cc = cur->_cc.resume();
    } else {
        cur->_realRun();
    }
    // 协程结束, 回到main loop
    CoFrame::setCurrentRoutine(NULL);

}

void CoFrame::addRunnable(Routine* r) {
    _runnables.push_back(r);
}
