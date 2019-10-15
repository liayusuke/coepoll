/*
 * Author : pufan
 * Created on : 2019-10-13
 * Contract : 517993945@qq.com
 */

#include <string>
#include <map>
#include <functional>

#include "coroutine.h"

bool setNonBlock(int fd);

class EntryRound: public Routine {

public:

    virtual void process();

    virtual void handle();

    void setReq(const std::string& req);

    std::string getReq();

    void setResp(const std::string& resp);

    std::string getResp();

private:

    std::string _req;

    std::string _resp;

    Routine*  _conn;

    friend class Connection;
};

class UDPEntry: public Routine {

public:

    UDPEntry(uint16_t port, std::function<EntryRound*()> creator);

    bool setup();

    void virtual process();

private:

    std::function<EntryRound*()> _creator;

    uint16_t _port;

    int _sockfd;
};

class Connection: public Routine {

public:

    Connection(int fd, std::function<EntryRound*()> creator);

    void virtual process();

private:

    int _fd;

    std::function<EntryRound*()> _creator;
};

class TCPEntry: public Routine {

public:

    TCPEntry(uint16_t port, std::function<EntryRound*()> creator);

    bool setup();

    void virtual process();

private:

    std::function<EntryRound*()> _creator;

    uint16_t _port;

    int _sockfd;
};

class EntryFrame {

public:

    static EntryFrame* ins();

    EntryFrame* addUDPEntry(uint16_t port, std::function<EntryRound*()> creator);

    EntryFrame* addTCPEntry(uint16_t port, std::function<EntryRound*()> creator);

    bool setup();

private:

    std::map<uint16_t, std::function<EntryRound*()> > _udpCreators;
    std::map<uint16_t, std::function<EntryRound*()> > _tcpCreators;
};
