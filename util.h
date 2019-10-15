/*
 * Author : pufan
 * Created on : 2019-10-08
 * Contract : 517993945@qq.com
 */

uint64_t getNowMs();


// stdout logging

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define LOG_DBUG(fmt, args...) printf(ANSI_COLOR_BLUE "DBUG " ANSI_COLOR_RESET "[%s:%d] [%s]" fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define LOG_INFO(fmt, args...) printf(ANSI_COLOR_GREEN "INFO " ANSI_COLOR_RESET "[%s:%d] [%s]" fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define LOG_WARN(fmt, args...) printf(ANSI_COLOR_YELLOW "WARN" ANSI_COLOR_RESET "[%s:%d] [%s]" fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define LOG_ERRO(fmt, args...) printf(ANSI_COLOR_RED "ERRO " ANSI_COLOR_RESET "[%s:%d] [%s]" fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
