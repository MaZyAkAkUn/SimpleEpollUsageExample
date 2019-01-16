#include "server.h"



Server::Server(unsigned short int lport, int wcount)
{
    listen_port = lport;
    working_thread_count = wcount;
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(listen_port);
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_sock = socket(bind_addr.sin_family, SOCK_STREAM, 0);
    if(listen_sock < 0){
        perror("Socket creating error");
        exit(EXIT_FAILURE);
    }

    if(bind(listen_sock, (struct sockaddr*)&bind_addr, sizeof(bind_addr)) < 0){
        perror("Socket binding");
        exit(1);
    }

    if(listen(listen_sock, 10000) < 0){
        perror("Socken set listen");
        exit(1);
    }
    std::cout<<"Starting server on port "<<listen_port<<"."<<std::endl;
    std::cout<<"Servers sock: "<<listen_sock<<std::endl;
    start_main_thread_poll();

}

void Server::run(){
    while(true){
        int current_cient_sock = accept(listen_sock, nullptr, nullptr);
        main_epoll_event.events = EPOLLIN|EPOLLONESHOT;
        main_epoll_event.data.fd = current_cient_sock;
        if(epoll_ctl(main_epoll_desc, EPOLL_CTL_ADD, current_cient_sock, &main_epoll_event) < 0){
            perror("Main epoll add");
            exit(1);
        }
    }
}

void parse_mess(char* mess)
{
    std::cout<<"Client message: "<< mess <<std::endl;
    Json::Value mess_json;
    Json::Reader mess_reader;

    if(!mess_reader.parse(mess, mess_json, false)){
        std::cout<<"Message parsing error!"<<std::endl;
        return;
    }
    std::cout<<mess_json["user"].asCString()<<": "<<mess_json["message"]<<std::endl;



}

void Server::process_client_conn(int epoll_desc)
{
    std::cout<<"Workers id: "<<std::this_thread::get_id()<<std::endl;
    struct epoll_event events[1];
    char mess_buff[MAX_BUFF_SIZE];

//    while (true) {
//        int events_count = epoll_wait(main_epoll_desc, events, 1, 5000);
//        if(events_count > 0)
//        {
//            int current_client_socket = events[0].data.fd;
//            std::cout<<"Event happened! Thread: "<<std::this_thread::get_id()<<". Client id: "<<current_client_socket<<std::endl;
//            int bytes_read = recv(current_client_socket, mess_buff, MAX_BUFF_SIZE, 0);
//            if(bytes_read < 0){
//                perror("Message reading");
//                close(current_client_socket);
//                continue;
//            }
//            parse_mess(mess_buff);
//            bzero(mess_buff, sizeof (mess_buff));

//        }
//    }

    while (true) {
               //call epoll_wait() and process it
               //std::cout<<"In thread loop started"<<std::endl;
               int events_count = epoll_wait(main_epoll_desc, events, 1, 5000);
               //std::cout<<"Waiting for events..."<<std::endl;
               if(events_count > 0)
               {
                   int client_socket = events[0].data.fd;

                   std::cout<<"Event happened, client socket: "<<client_socket<<std::endl;

                   int bytes_read = recv(client_socket, mess_buff, MAX_BUFF_SIZE, 0);
                   if(bytes_read <= 0){
                       //something got wrong...

                       close(client_socket);
                       bzero(mess_buff, sizeof (mess_buff));
                       perror("Byte reading");

                       continue;

                   }
                   parse_mess(mess_buff);
                   //send_mess(client_socket, "OK");
                   //TODO: upgrade parser, send mess to all clients and etc...

                   bzero(mess_buff, sizeof (mess_buff));


               }
            }
}


void Server::start_main_thread_poll()
{
    main_epoll_desc = epoll_create1(0);
    if(main_epoll_desc < 0){
        perror("Epoll desc creating");
        exit(1);
    }
    std::cout<<"efr"<<working_thread_count<<std::endl;

    for(int i = 0; i < working_thread_count; i++){
        std::thread worker(&Server::process_client_conn, this, main_epoll_desc);
        worker.detach();
        std::cout<<"Thread created"<<std::endl;
    }


}

Server::~Server()
{

}






















