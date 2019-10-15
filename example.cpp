/*
 * Author : pufan
 * Created on : 2019-10-14
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



#include "entry.h"
#include "co_udp.h"
#include "util.h"
#include "coroutine.h"
#include "co_tcp.h"

class UServer1: public EntryRound {

public:

    virtual void process() {

        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

        LOG_WARN("userver 1");
        if (sockfd < 0) {
            LOG_ERRO("socket error");
            return;
        }

        struct sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_port = 10001;
        sin.sin_addr.s_addr = INADDR_ANY;

        int n, len;

        const char* hello = "req[10000]";
        n = udp::sendto(sockfd, hello, strlen(hello), 0, (const struct sockaddr*)&sin, sizeof(sin), 0);
        if (n < 0) {
            LOG_ERRO("sendto error");
            return;
        }

        char buf[1024] = {0};
        n = udp::recvfrom(sockfd, buf, 1024, 0, (struct sockaddr*)&sin, (socklen_t*)&len, 0);

        this->setResp(std::string("resp[10000]") + std::string(buf, n));

    }
};

class UServer2: public EntryRound {

public:

    virtual void process() {
    }
};

class TServer1: public EntryRound {

public:

    virtual void handle() {
        char buf[24] = {0};
        snprintf(buf, 24, "resp:[%p]", this);
        this->setResp(std::string(buf)+ getReq());
    }
};

class TServer2: public EntryRound {

public:

    virtual void handle() {
        int sockfd;
        struct sockaddr_in sin;

        sockfd = socket(PF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            LOG_ERRO("socket error");
            return;
        }

        setNonBlock(sockfd);

        sin.sin_port = htons(12346);
        sin.sin_family = PF_INET;
        sin.sin_addr.s_addr = inet_addr("127.0.0.1");

        LOG_DBUG("connect 12346");
        if (tcp::connect(sockfd, (struct sockaddr*)&sin, sizeof(sin), 1000) < 0) {
            LOG_ERRO("connect error");
            return;
        }

        std::string req = this->getReq();
        int n = tcp::send(sockfd, req.data(), req.size(), 0, 1000);
        if (n < 0) {
            LOG_ERRO("send error");
            return;
        }
        LOG_DBUG("send 12346 %d", n);

        char buf[1024] = {0};

        n = tcp::recv(sockfd, buf, sizeof(buf), 0, 1000);
        if (n < 0) {
            LOG_ERRO("recv error");
            return;
        }
        LOG_DBUG("recv 12346 %d", n);

        char out[1024] = {0};
        snprintf(out, sizeof(out), "resp[%p]:%s", this, std::string(buf).c_str());
        close(sockfd);
        this->setResp(std::string(out));

    }

};

int main() {

    if (!CoFrame::ins()->init(new CoDaemon)) {
        LOG_ERRO("coframe init error");
        return -1;
    }

    EntryFrame::ins()->addTCPEntry(12345, [](){
            return new TServer2;
            })->addTCPEntry(12346, [](){
                return new TServer1;
                });

    if (!EntryFrame::ins()->setup()) {
        LOG_ERRO("entryframe setup error");
    }

    for (;;) {
        CoFrame::ins()->schedule();
    }

    return 0;
}
