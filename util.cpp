/*
 * Author : pufan
 * Created on : 2019-10-08
 * Contract : 517993945@qq.com
 */


#include <chrono>

#include "util.h"

uint64_t getNowMs() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
