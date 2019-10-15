/*
 * Author : pufan
 * Created on : 2019-10-13
 * Contract : 517993945@qq.com
 */

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <fcntl.h>

#include "entry.h"
#include "util.h"
#include "co_udp.h"
#include "co_tcp.h"
#include "coroutine.h"

#define BUFFER_LENGTH 65536

bool setNonBlock(int fd) {
    int opts = fcntl(fd, F_GETFL);
    if (opts < 0) {
        LOG_ERRO("fcntl get error");
        return false;
    }

    opts = opts | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, opts) < 0) {
        LOG_ERRO("fcntl set error, %d, %s", errno, strerror(errno));
        return false;
    }
    return true;
}

void EntryRound::process() {

    handle();
    CoFrame::ins()->addRunnable(_conn);
}

void EntryRound::handle() {

}

void EntryRound::setReq(const std::string& req) {
    _req = req;
}

std::string EntryRound::getReq() {
    return _req;
}

void EntryRound::setResp(const std::string& resp) {
    _resp = resp;
}

std::string EntryRound::getResp() {
    return _resp;
}

UDPEntry::UDPEntry(uint16_t port, std::function<EntryRound*()> creator) {
    _port = port;
    _creator = creator;
}

bool UDPEntry::setup() {

    struct sockaddr_in sin;

    if ((_sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        LOG_ERRO("socket error");
        return false;
    }

    if (!setNonBlock(_sockfd)) {
        LOG_ERRO("set non block error");
        return false;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = PF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(_port);

    if (bind(_sockfd, (const struct sockaddr*)&sin, sizeof(sin)) < 0) {
        LOG_ERRO("bind error");
        return false;
    }
    LOG_DBUG("bind socket %u", _port);

    return true;
}

void UDPEntry::process() {

    char* buf = new char[BUFFER_LENGTH];

    for (;;) {

        struct sockaddr_in cliAddr;

        memset(&cliAddr, 0, sizeof(cliAddr));
        memset(buf, 0, BUFFER_LENGTH);

        int len;
        int n = udp::recvfrom(_sockfd, buf, BUFFER_LENGTH, 0, (struct sockaddr*)&cliAddr, (socklen_t*)&len, 0);
        if (n < 0) {
            LOG_ERRO("recvfrom error %d", n);
            continue;
        }

        EntryRound* er = _creator();
        er->setReq(std::string(buf, n));
        CoFrame::ins()->addRunnable(er);
        CoFrame::ins()->schedule();

        // set req/ get resp
        
        std::string resp = er->getResp();

        if (!resp.empty()) {
            n = udp::sendto(_sockfd, resp.data(), resp.size(), 0, (struct sockaddr*)&cliAddr, len, 0);
            if (n < 0) {
                LOG_ERRO("sento error");
                delete er;
                continue;
            }
        }

        delete er;
    }
}

Connection::Connection(int fd, std::function<EntryRound*()> creator) {
    _fd = fd;
    _creator = creator;
}

void Connection::process() {

    char* buf = new char[BUFFER_LENGTH];

    for (;;) {

        memset(buf, 0, BUFFER_LENGTH);

        int n = tcp::recv(_fd, buf, BUFFER_LENGTH, 0, 0);
        if (n < 0) {
            LOG_ERRO("recv error");
            continue;
        }

        if (n == 0) {
            close(_fd);
            delete buf;
            return;
        }

        EntryRound* er = _creator();
        LOG_DBUG("new entry %p", er);
        er->setReq(std::string(buf, n));
        er->_conn = this;
        CoFrame::ins()->addRunnable(er);
        CoFrame::ins()->schedule();

        std::string resp = er->getResp();
        if (!resp.empty()) {
            if (tcp::send(_fd, resp.data(), resp.size(), 0, 1000) < 0) {
                LOG_ERRO("tcp send error");
            }
        }
        delete er;
    }
}

TCPEntry::TCPEntry(uint16_t port, std::function<EntryRound*()> creator) {
    _port = port;
    _creator = creator;
}

bool TCPEntry::setup() {

    struct sockaddr_in sin;

    if ((_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        LOG_ERRO("socket error");
        return false;
    }

    if (!setNonBlock(_sockfd)) {
        LOG_ERRO("set non block error");
        return false;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = PF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(_port);

    if (bind(_sockfd, (const struct sockaddr*)&sin, sizeof(sin)) < 0) {
        LOG_ERRO("bind error");
        return false;
    }
    LOG_DBUG("bind socket %u", _port);

    if (listen(_sockfd, 1024) < 0) {
        LOG_ERRO("listen error");
        return false;
    }

    return true;
}

void TCPEntry::process() {

    char* buf = new char[BUFFER_LENGTH];

    for (;;) {

        memset(buf, 0, BUFFER_LENGTH);

        int cli = tcp::accept(_sockfd, NULL, NULL, 0);
        if (cli < 0) {
            LOG_ERRO("accept error");
            continue;
        }

        setNonBlock(cli);

        Connection* conn = new Connection(cli, _creator);
        CoFrame::ins()->addRunnable(conn);
    }
}


EntryFrame* EntryFrame::ins() {

    static EntryFrame* _ins = NULL;
    if (!_ins) {
        _ins = new EntryFrame;
    }

    return _ins;
}

EntryFrame* EntryFrame::addUDPEntry(uint16_t port, std::function<EntryRound*()> creator) {

    _udpCreators[port] = creator;
    return this;
}

EntryFrame* EntryFrame::addTCPEntry(uint16_t port, std::function<EntryRound*()> creator) {

    _tcpCreators[port] = creator;
    return this;
}

bool EntryFrame::setup() {

    for (auto& u : _udpCreators) {
        UDPEntry* ent = new UDPEntry(u.first, u.second);
        if (!ent->setup()) {
            return false;
        }

        CoFrame::ins()->addRunnable(ent);
    }

    for (auto& u : _tcpCreators) {
        TCPEntry* ent = new TCPEntry(u.first, u.second);
        if (!ent->setup()) {
            return false;
        }

        CoFrame::ins()->addRunnable(ent);
    }
    return true;
}
