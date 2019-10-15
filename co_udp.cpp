/*
 * Author : pufan
 * Created on : 2019-10-08
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

#include "co_udp.h"
#include "coroutine.h"
#include "util.h"

namespace udp{

int sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen, int timeout) {

    int n = 0;
    uint64_t start = getNowMs();
    while((n = ::sendto(sockfd, buf, len, flags, dest_addr, addrlen)) < 0) {

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

        LOG_INFO("sendto call schedule");
        CoFrame::ins()->schedule();
        LOG_INFO("sendto after schedule");

        if (!CoFrame::ins()->getPoller()->del(sockfd)) {
            LOG_ERRO("del fd error %d", sockfd);
            return -1;
        }
    }

    return n;
}

int recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen, int timeout) {
    int n = 0;
    uint64_t start = getNowMs();
    while((n = ::recvfrom(sockfd, buf, len, flags, src_addr, addrlen)) < 0) {

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

        if (!CoFrame::ins()->getPoller()->add(sockfd, EPOLLIN|EPOLLHUP|EPOLLERR, CoFrame::ins()->getCurrentRoutine())) {
            LOG_ERRO("add fd error %d", sockfd);
            return -1;
        }

        LOG_INFO("recvfrom call schedule");
        CoFrame::ins()->schedule();
        LOG_INFO("recvfrom after schedule");

        if (!CoFrame::ins()->getPoller()->del(sockfd)) {
            LOG_ERRO("del fd error %d", sockfd);
            return -1;
        }
 
    }

    return n;
}

}
