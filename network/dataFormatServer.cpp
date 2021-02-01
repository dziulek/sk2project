#include "dataFormatServer.hpp"

void DataFormatServer::appendPlayer(agario::Player * player){

    this->appendChar(PLAYER);

    this->appendChar(STATE);

    this->appendChar(player->getState() == false ? '0' : '1');

    this->appendChar(NICKNAME);

    this->appendString("Unnamed_cell");

    this->appendChar(COORDINATES);

    for(int i = 0; i < player->getSize(); i++){

        // std::cout<< "x: " << (*player)[i].getPosition().x << ", y: " << (*player)[i].getPosition().y <<std::endl;
        // std::cout << "mass: " << (*player)[i].getArea() <<std::endl;

        this->appendFloat((*player)[i].getPosition().x);
        this->appendFloat((*player)[i].getPosition().y);
        this->appendFloat((*player)[i].getRadius());
    }
}

void DataFormatServer::appendView(agario::Player * player){

    this->appendChar(VIEW);

    std::pair<glm::vec2, glm::vec2> view = player->getView();

    this->appendFloat(view.first.x);
    this->appendFloat(view.first.y);
    this->appendFloat(view.second.x);
    this->appendFloat(view.second.y);
}

void DataFormatServer::appendMass(agario::Player * player){

    this->appendChar(MASS);

    this->appendFloat(player->getTotalArea());
}

void DataFormatServer::extractClientInfo(clientInfo & cinfo){

    char c;
    float x, y;
    bool b;

    int curr_ind = 0;

    while(curr_ind < this->getLen() - 1){

        c = this->getChar(curr_ind);

        if(c == MOUSE){
            curr_ind = this->getNextIndexSeparator(curr_ind);
            x = this->getFloat(curr_ind);
            curr_ind = this->getNextIndexSeparator(curr_ind);
            y = this->getFloat(curr_ind);
            cinfo.mousePosition = {x, y};

            c = '-';
        }
        else if(c == W_MASS){
            curr_ind = this->getNextIndexSeparator(curr_ind);
            char t = this->getChar(curr_ind);
            cinfo.w_action = (t == '0' ? false : true);

            c = '-';
        }
        else if(c == DIVIDE_ACTION){
            curr_ind = this->getNextIndexSeparator(curr_ind);
            char t = this->getChar(curr_ind);
            cinfo.divide_action = (t == '0' ? false : true);

            c = '-';
        }
        else if(c == STATE){
            curr_ind = this->getNextIndexSeparator(curr_ind);
        }
        curr_ind = this->getNextIndexSeparator(curr_ind);
    }
}

void DataFormatServer::appendMinis(agario::Game * game, agario::Player * player){

    std::pair<std::pair<int, int>, std::pair<int, int>> mini_range = game->getMap()->getMiniRects(
        player->getView().first, player->getView().second
    );

    // std::cout<<mini_range.first.first<<" "<<mini_range.first.second<<std::endl;
    // std::cout<<mini_range.second.first<<" "<<mini_range.second.second<<std::endl;
    this->appendChar(MINIS);

    for(int i = mini_range.first.first; i < mini_range.second.first; i++){

        for(int j = mini_range.first.second; i < mini_range.second.second; i++){

            for(auto & m : game->getMap()->minis[j][i]){
                
                this->appendFloat(m.get()->getPosition().x);
                this->appendFloat(m.get()->getPosition().y);
                this->appendFloat(m.get()->getRadius());
            }    
        }
    }
}