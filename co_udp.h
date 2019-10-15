/*
 * Author : pufan
 * Created on : 2019-10-08
 * Contract : 517993945@qq.com
 */

namespace udp{

int sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen, int timeout);

int recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen, int timeout);

}
