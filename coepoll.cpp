/*
 * Author : pufan
 * Created on : 2019-10-08
 * Contract : 517993945@qq.com
 */

#include <iostream>
#include <boost/context/continuation.hpp>

#include "util.h"

namespace ctx = boost::context;

ctx::continuation foo(ctx::continuation&& c) {
    do {
        LOG_INFO("foo");
    }while((c = c.resume()));
    LOG_INFO("return");
    return std::move(c);
}

int main() {
    ctx::continuation c = ctx::callcc(foo);
    
    do {
        LOG_ERRO("bar");
    } while((c = c.resume()));

    std::cout << "main: done" << std::endl;
    return 0;
}
