#include "game.h"
#include "common_def.h"

//if two players, then put them in different camp, A and B
const std::string TWO_PLAYER_START_GAME_STRING = "0,A#1,B";

//if four players, then first 2 are in camp A, second 2 are in camp B
const std::string FOUR_PLAYER_START_GAME_STRING = "0,A#1,A#2,B#3,B";

/**********************************************************************/

void Game::SendStartGameNtfToAll()
{
    std::string to_send;
    std::string to_send_end;
    if(m_player_list->size() == 2)
    {
        to_send_end = TWO_PLAYER_START_GAME_STRING;
    }
    else if(m_player_list->size() == 4)
    {
        to_send_end = FOUR_PLAYER_START_GAME_STRING;
    }

    int player_idx = 0;
    //each player
    for(auto session_iter = m_player_list->begin(); session_iter != m_player_list->end(); ++session_iter)
    {
        //SocketPtr sock = (*session_iter)->GetSocketPtr();
        tcp::socket& sock = (*session_iter)->FspSocket();

        //STARTGAME#max_players_num#my_idx#0,A#1,A#2,B#3,B\n
        to_send = "STARTGAME#" + std::to_string(m_player_list->size()) + "#"
            + std::to_string(player_idx++) +"#" + to_send_end + "\n";

        boost::asio::async_write(sock,
                                     boost::asio::buffer(to_send, to_send.length()),
                                     boost::bind(&Game::HandleWrite, this,
                                                 boost::asio::placeholders::error));
    }
}

void Game::HandleWrite(const boost::system::error_code& error)
{
    if(error)
    {
        printf("ERROR: SendStarGameNtfToAll Error!");
        return;
    }
}
