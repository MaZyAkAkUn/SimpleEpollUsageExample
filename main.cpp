#include <iostream>
#include <unistd.h>
#include "jsoncpp/json/json.h"
#include <fstream>
#include <sstream>
#include "server.h"
int main(int argc, char** argv)
{
    std::ifstream conf_file("config.json");
    if(!conf_file.is_open()){
        perror("Open conf file");
        exit(1);
    }
    Json::Value conf_value;
    Json::Reader conf_reader;
    conf_reader.parse(conf_file, conf_value, false);
    if(conf_value["use_hw_conc"].asInt() == 0){
        Server* server = new Server(conf_value["listen_port"].asUInt(), conf_value["max_thread_count"].asInt());
        server->run();
    }
    else{
        Server* server = new Server(conf_value["listen_port"].asUInt(), std::thread::hardware_concurrency());
        server->run();
    }

    return 0;
}
