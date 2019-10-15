/*
 * Author : pufan
 * Created on : 2019-10-09
 * Contract : 517993945@qq.com
 */

#include <vector>

struct Fly {
    void* ptr;
    int fd;
};

class Epoll {

public:

    bool init();

    bool run(int timeout);

    bool add(int fd, int events, void* ptr);

    bool del(int fd);

    std::vector<Fly> getRunnable();

private:

    int _efd;

    std::vector<Fly> _runnables;
};
