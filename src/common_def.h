#ifndef _COMMON_DEF_H_
#define _COMMON_DEF_H_

#include <unordered_map>
#include <cstdio>
#include <stddef.h>
#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;
using namespace std;

//use boost shared ptr to wrap socket
typedef boost::shared_ptr<tcp::socket> SocketPtr;

static const int kMaxPkgSize= 1024;

static const int kMaxActionNumInOneFrame = 40; //一帧之内玩家操作的数量上限

static const size_t kUdpSegmentSize = 400;      //每一次下发包体不超过400字节
static const int kMaxFrameBufInGame = 512000;  //一帧的最大容量
static const int kUdpPkgHeadSize = 20 + 8;
static const int kMaxSegmentSize =  2 * kMaxFrameBufInGame / kUdpSegmentSize; //这个用kMaxSegmentCnt更好理解一点， bug

static const int kMaxKeySize = 16;

static const int kPickActionEveryPlayer = 5; // every time pick 5 action from each player 
#endif
