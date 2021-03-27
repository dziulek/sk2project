#include "threadFunctions.hpp"

/*
Tutaj znajdują się funkcje które są wywoływane przez serwer w osobnych wątkach.
*/

//Wątek uruchamiany dla każdego nowego klienta
void * clientThread(void * server_client_struct){

    server_client * sc = (server_client *)server_client_struct;
    
    Client * client = sc->server->addNewClient(sc->client_sockfd, sc->ip_addr, sc->s);

    DataFormatServer buf;
    clientInfo cinfo;

    while(client != nullptr && client->getDisconnect() == false){

        buf.clearBuf();

        int status = read(client->getSockfd(), buf.getBuf(), 150);

        if(status == 0){
            //closed socket
            break;
        }
        // buf.printBuf();

        if(buf.getWord(0) == "data"){

            if(client->getPlayer() == nullptr || client->getPlayer()->getSize() == 0){
                
                client->getPlayer()->setState(false);
            }

            buf.extractClientInfo(cinfo);
            client->getGame()->setPlayerMousePosition(client->getPlayer(), cinfo.mousePosition);

            int status = sc->server->sendDataToClient(client);

            if(status == -1){
                fprintf(stdout, "client disconnected: %s\n", client->getIp_addr());
            }
            

        }
        else if(strcmp("get:score", buf.getBuf()) == 0){

        }
        else if(buf.getWord(0) == "nickname"){

            int ind = buf.getNextIndexSeparator(0);
            std::string nick = buf.getWord(ind);
            std::cout << nick << std::endl;

            client->getPlayer()->setNickname(nick);
            client->getPlayer()->setColor();

            buf.clearBuf();

            buf.appendSeparator();
            buf.appendFloat(client->getGame()->getMap()->width);
            buf.appendFloat(client->getGame()->getMap()->height);
            
            int status = write(client->getSockfd(), buf.getBuf(), buf.getLen());

            if(status == -1){
                fprintf(stdout, "client disconnected: %s\n", client->getIp_addr());
            }
        }
        
    }

    pthread_mutex_lock(&sc->server->client_creation_mutex);

    pthread_mutex_lock(&sc->server->new_player_mutex);

    client->setDisconnect();
    client->getGame()->getMap()->playerObjectAbandoned(client->getPlayer());
    client->getGame()->deletePlayer(client->getPlayer());

    pthread_mutex_unlock(&sc->server->new_player_mutex);
    close(client->getSockfd());
    sc->server->cullDisconnectedClients();

    pthread_mutex_unlock(&sc->server->client_creation_mutex);

    std::cout<<"exit from client thread"<<std::endl;
    pthread_exit(NULL);
}

//Wątek obsługujący gry
void * gameThread(void * srv){

    const int deltaTime = 16666;
    Server * server = (Server *)srv;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    while(server->close_server == false){

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        int delta = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        if(delta > deltaTime){
            begin = std::chrono::steady_clock::now();
            server->gameLoop(static_cast<float>(delta) * 1e-6);
        }
    }

    fprintf(stdout, "exit from game thread\n");
    pthread_exit(NULL);
}

//Wątek serwera, służy do kontroli stanu serwera, bezpiecznego zamknięcia itd.
void * serverInfoRoutine(void * args){

    std::chrono::steady_clock::time_point lastControl = std::chrono::steady_clock::now();

    Server * server = (Server *)args;

    int terminate = true;
    std::string s;

    while(terminate){

        std::cin>>s;
        
        if(s == "clients")
        {
            fprintf(stdout, "current number of clients connected: %d\n", (int)server->clients.size());
        }
        else if(s == "games")
        {
            fprintf(stdout, "number of games running: %d\n", (int)server->games.size());
            for(int i = 0; i < server->games.size(); i++){
                std::cout << "Board " << i + 1 << ":\n";
                pthread_mutex_lock(&server->new_player_mutex);//======================player

                if(server->games[i].get()->getnOfPlayers() == 0)
                    fprintf(stdout, "brak graczy\n");
                else
                    for(int j = 0; j < server->games[i].get()->getnOfPlayers(); j++){
                        fprintf(stdout, "-- %s\n", server->games[i]->getPlayer(j).getNickname().c_str());
                }

                pthread_mutex_unlock(&server->new_player_mutex);//====================player
            }
        }
        else if(s == "port"){
            fprintf(stdout, "server is running on %s port\n", server->portNumber);
        }
        else if(s == "cullClients"){

            server->cullDisconnectedClients();
            fprintf(stdout, "deleting disconnected clients\n");
        }
        else if(s == "time"){
            
            std::time_t time = server->getServerTime();

            std::cout<<std::ctime(&time)<<std::endl;
        }
        if(s == "closeServer:4rfvbgt5"){

            server->close_server = true;

            for(auto & c : server->clients){
                
                c.get()->setDisconnect();
                server->disconnectClient(c.get()->getSockfd());
            }
            break;
        }
    }



    fprintf(stdout, "exit from infoServerRoutine thread\n");
    pthread_exit(NULL);
}