#ifndef SERVER_HPP
#define SERVER_HPP

#include "../engine/agario_engine.hpp"
#include "threadFunctions.hpp"
#include "client.hpp"
#include "constants.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <vector>
#include <memory>
#include <map>

class Server;

typedef void * (*THREADFUNCPTR)(void *);

struct sendDataFormat{

    char state;
    float window_coordinates[2][2];
    float player_coordinates[16][2];
    float other_players_coordinates[5][16][2];
    float minis_coorindate[100][2];
    float bomb_coordinates[20][2];
};

struct recvDataFormat{

    char state;
    bool w_action;
    bool divide_action;
    int mouse_coordinates[2];
};

struct server_client{
    Server * server;
    char * ip_addr;
    int client_sockfd;
    sockaddr_storage * s;
};


class Server{

private:

    std::vector<std::unique_ptr<agario::Game>> games;
    std::vector<std::unique_ptr<Client>> clients;

    char portNumber[5];
    int sockfd;
    struct addrinfo hints;
    struct addrinfo *serverInfo;
    int opt_value = 1;
    char s[INET_ADDRSTRLEN];

    bool close_server = false;

    pthread_t server_thread;
    pthread_t send_thread;
    pthread_mutex_t send_data_mutex = PTHREAD_MUTEX_INITIALIZER;

    int setUpServer();
    int sendDataToClient(Client * client);
    void * get_in_addr(struct sockaddr *sa);
    void findGameForNewClient(Client * client);
    void interpretData(recvDataFormat * data);

    void * serverInfoRoutine(void * args);
    void fillDataToClient(Client * client, sendDataFormat & sendData);
    void * sendDataThread(void * args);
    
public:

    Server(){
        
        strcpy(this->portNumber, std::string("1234").c_str());
        int status = setUpServer();
        if(status < 0){

        }
    }
    Server(std::string portNumber){
        
        int status = setUpServer();
        if(status < 0){

        }
    }

    Client * addNewClient(int sockfd, char * ip_addr, struct sockaddr_storage * s);
    int disconnectClient(int sockfd);
    void createNewGame();
    void closeServer();
    void deleteGame(std::unique_ptr<agario::Game> & game);
    void deleteGame(int gameIndex);
    void deleteEmptyGames();
    void * sendDataToClients(void * args);
    void * listenOnSocket(void * client);
    friend void * clientThread(void * server_client);

    int mainLogic();
};




#endif