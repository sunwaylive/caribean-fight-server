#include "game.h"
#include "common_def.h"

//if two players, then put them in different camp, A and B
const std::string TWO_PLAYER_START_GAME_STRING = "0,A#1,B";
//if four players, then first 2 are in camp A, second 2 are in camp B
const std::string FOUR_PLAYER_START_GAME_STRING = "0,A#1,A#2,B#3,B";

/**********************************************************************/
int Game::FrameTick()
{
    //way 1: use Fsp
    //FspCalculateFrame(); //this will modify m_cur_frame
    //m_frame_mgr.AddFrame(&m_cur_frame);
    //SendFrameNtfToAll();
    //m_cur_frame.clear();
    
    //way 2: use Ssp
    SspCalculateFrame();  //this will modify m_cur_state
    m_frame_mgr.AddState(m_cur_state);
    SendStateNtfToAll();
    m_cur_state.clear();

    return 0;
}

int Game::SspCalculateFrame()
{
    //pick the newest state of each client
    for(auto session_iter = m_player_list->begin(); session_iter != m_player_list->end(); ++session_iter)
    {
        Session *s = (*session_iter); 

        std::string state;
        if(s->PopState(state) == 0)
        {
            m_cur_state += state;
        }
    }
    
    return 0;
}

void Game::SendStateNtfToAll()
{
    for(auto session_iter = m_player_list->begin(); session_iter != m_player_list->end(); ++session_iter)
    {
        (*session_iter)->SendStateCacheToClient(&m_frame_mgr);
    }
}

int Game::FspCalculateFrame()
{
    vector<Session*> sess_list_tmp;
    sess_list_tmp.insert(sess_list_tmp.end(), m_player_list->begin(), m_player_list->end());

    //read every player's input
    for(auto session_iter = sess_list_tmp.begin(); session_iter != sess_list_tmp.end(); )
    {
        Session *s = *session_iter;

        Action action;
        bool erase = false; //use for move session iterator
        int count = 0;
        while (count < kPickActionEveryPlayer)
        {
            if (m_cur_frame.IsFull()) //multiple jump, forgive me
            {
                goto pick_action_end;
            }

            if(s->PopAction(action) == 0)
            {
                this->AddActionToCurFrame(action);
                ++count;
            }
            else
            {
                session_iter = sess_list_tmp.erase(session_iter);
                erase = true;
                break;
            }
        }

        if(!erase)
        {
            ++session_iter ;
        }
    }

pick_action_end:
    return 0;
}

int Game::AddActionToCurFrame(Action action)
{
    //BUG
    return 0;
}

void Game::SendFrameNtfToAll()
{
    for(auto session_iter = m_player_list->begin(); session_iter != m_player_list->end(); ++session_iter)
    {
        (*session_iter)->SendFrameCacheToClient(&m_frame_mgr);
    }
}

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
    printf("player num: %lu\n", m_player_list->size());

    //each player
    for(auto session_iter = m_player_list->begin(); session_iter != m_player_list->end(); ++session_iter)
    {
        tcp::socket& sock = (*session_iter)->GameSocket();

        //STARTGAME#max_players_num#my_idx#0,A#1,A#2,B#3,B\n
        to_send = "STARTGAME#" + std::to_string(m_player_list->size()) + "#"
            + std::to_string(player_idx++) +"#" + to_send_end + "\n";
        cout<<"player index: " << player_idx <<" to send: " << to_send <<endl;
        boost::asio::async_write(sock,boost::asio::buffer(to_send, to_send.length()),
                                      boost::bind(&Game::HandleWrite, this,
                                                  boost::asio::placeholders::error));
    }
}

void Game::HandleWrite(const boost::system::error_code& error)
{
    if(!error)
    {
    }
    else
    {
        cout<<"ERROR: SendStartGameNtfToAll " << error.message() <<endl;
        return;
    }
}
