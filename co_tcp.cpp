/*
 * Author : pufan
 * Created on : 2019-10-15
 * Contract : 517993945@qq.com
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#include "co_tcp.h"
#include "coroutine.h"
#include "util.h"


namespace tcp{

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen, int timeout) {

    uint64_t start = getNowMs();

    while (::connect(sockfd, addr, addrlen) < 0) {

        if (timeout > 0) {
            if (getNowMs() > start + timeout) {
                return -1;
            }
        }

        if (EINTR == errno) {
            continue;
        }

        if (EAGAIN != errno && EINPROGRESS != errno) {
            LOG_ERRO("errno %d", errno);
            return -1;
        }

        if (!CoFrame::ins()->getPoller()->add(sockfd, EPOLLOUT, CoFrame::ins()->getCurrentRoutine())) {
            LOG_ERRO("add fd error %d", sockfd);
            return -1;
        }

        LOG_INFO("call schedule");
        CoFrame::ins()->schedule();
        LOG_INFO("after schedule");

        if (!CoFrame::ins()->getPoller()->del(sockfd)) {
            LOG_ERRO("del fd error %d", sockfd);
            return -1;
        }
 
    }

    return 0;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int timeout) {

    int n = 0;
    uint64_t start = getNowMs();
    while((n = ::accept(sockfd, addr, addrlen)) < 0) {

        if (timeout > 0) {
            if (getNowMs() > start + timeout) {
                return -1;
            }
        }

        if (EINTR == errno) {
            continue;
        }

        if (EAGAIN != errno) {
            LOG_ERRO("errno %d", errno);
            return -1;
        }

        if (!CoFrame::ins()->getPoller()->add(sockfd, EPOLLIN, CoFrame::ins()->getCurrentRoutine())) {
            LOG_ERRO("add fd error %d", sockfd);
            return -1;
        }

        LOG_INFO("call schedule");
        CoFrame::ins()->schedule();
        LOG_INFO("after schedule");

        if (!CoFrame::ins()->getPoller()->del(sockfd)) {
            LOG_ERRO("del fd error %d", sockfd);
            return -1;
        }
    }

    return n;

}

int recv(int sockfd, void *buf, size_t len, int flags, int timeout) {

    int n = 0;
    uint64_t start = getNowMs();
    while((n = ::recv(sockfd, buf, len, flags)) < 0) {

        if (timeout > 0) {
            if (getNowMs() > start + timeout) {
                return -1;
            }
        }

        if (EINTR == errno) {
            continue;
        }

        if (EAGAIN != errno) {
            LOG_ERRO("errno %d", errno);
            return -1;
        }

        if (!CoFrame::ins()->getPoller()->add(sockfd, EPOLLIN, CoFrame::ins()->getCurrentRoutine())) {
            LOG_ERRO("add fd error %d", sockfd);
            return -1;
        }

        LOG_INFO("call schedule");
        CoFrame::ins()->schedule();
        LOG_INFO("after schedule");

        if (!CoFrame::ins()->getPoller()->del(sockfd)) {
            LOG_ERRO("del fd error %d", sockfd);
            return -1;
        }
    }

    return n;

}

int send(int sockfd, const void *buf, size_t len, int flags, int timeout) {

    int n = 0;
    uint64_t start = getNowMs();
    while((n = ::send(sockfd, buf, len, flags)) < 0) {

        if (timeout > 0) {
            if (getNowMs() > start + timeout) {
                return -1;
            }
        }

        if (EINTR == errno) {
            continue;
        }

        if (EAGAIN != errno) {
            LOG_ERRO("errno %d", errno);
            return -1;
        }

        if (!CoFrame::ins()->getPoller()->add(sockfd, EPOLLOUT, CoFrame::ins()->getCurrentRoutine())) {
            LOG_ERRO("add fd error %d", sockfd);
            return -1;
        }

        LOG_INFO("call schedule");
        CoFrame::ins()->schedule();
        LOG_INFO("after schedule");

        if (!CoFrame::ins()->getPoller()->del(sockfd)) {
            LOG_ERRO("del fd error %d", sockfd);
            return -1;
        }
    }

    return n;

}

}
