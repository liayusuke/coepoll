/*
 * Author : pufan
 * Created on : 2019-10-09
 * Contract : 517993945@qq.com
 */

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "epoll.h"

bool Epoll::init() {

    if ((_efd = epoll_create(1024)) < 0) {
        LOG_ERRO("epoll create %s", strerror(errno));
        return false;
    }
    return true;
}

bool Epoll::add(int fd, int events, void* ptr) {
    struct epoll_event ev = {};
    memset(&ev, 0, sizeof(ev));

    ev.events = events;
    ev.data.fd = fd;
    ev.data.ptr = ptr;

    if (epoll_ctl(_efd, EPOLL_CTL_ADD, fd, &ev) != 0) {
        LOG_ERRO("epoll_ctl %s", strerror(errno));
        return false;
    }

    LOG_DBUG("add");
    return true;
}

bool Epoll::del(int fd) {
    if (epoll_ctl(_efd, EPOLL_CTL_DEL, fd, NULL) != 0) {
        LOG_ERRO("epoll_ctl %s", strerror(errno));
        return false;
    }
    LOG_DBUG("del");
    return true;
}

bool Epoll::run(int timeout/*ms*/) {

    static const int maxEvents = 1024;
    struct epoll_event evs[maxEvents];

    int n = epoll_wait(_efd, evs, maxEvents, timeout);
    if (n < 0) {
        LOG_ERRO("epoll_wait %s", strerror(errno));
        return false;
    }

    LOG_DBUG("epoll_wait %d", n);

    for (int i = 0; i < n; i++) {
        int events = evs[i].events;

        if (events & EPOLLHUP) {
            LOG_ERRO("epoll hup");
        }

        if (events & EPOLLERR) {
            LOG_ERRO("epoll err");
        }

        if (events & EPOLLIN) {
            LOG_ERRO("epoll in");
        }

        if (events & EPOLLOUT) {
            LOG_ERRO("epoll out");
        }

        Fly fly = {evs[i].data.ptr, evs[i].data.fd};
        _runnables.push_back(fly);
    }

    return true;
}


std::vector<Fly> Epoll::getRunnable() {
    return std::move(_runnables);
}
