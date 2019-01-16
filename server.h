#ifndef SERVER_H
#define SERVER_H
#include <utilheader.h>

#define MAX_BUFF_SIZE 1200

class Server
{
public:
    Server(unsigned short int, int);//main init
    void run();//run main server loop, for start/restart and stoping server using python script or etc.
    ~Server();//cleaning all

    int working_thread_count;
    unsigned short int listen_port;
    int listen_sock;
    int main_epoll_desc;

    struct sockaddr_in bind_addr;
    struct epoll_event main_epoll_event;

    void process_client_conn(int);
    void start_main_thread_poll();


};

#endif // SERVER_H
