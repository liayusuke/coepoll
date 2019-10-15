/*
 * Author : pufan
 * Created on : 2019-10-15
 * Contract : 517993945@qq.com
 */

namespace tcp{

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen, int timeout); 

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int timeout);

int recv(int sockfd, void *buf, size_t len, int flags, int timeout);

int send(int sockfd, const void *buf, size_t len, int flags, int timeout);

}
