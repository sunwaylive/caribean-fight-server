#ifndef _UDP_PKG_DEF_H_
#define _UDP_PKG_DEF_H_
#include <stdint.h>
#pragma pack(1)

/*****************************/
typedef struct RecvAction
{
    uint16_t vkey;
    uint16_t arg;
    uint16_t client_frame_id;
}RecvAction;

typedef struct UdpPkgRecvHead
{
    uint16_t seq;
    uint16_t ack;
    uint16_t sid;
    uint8_t action_len;
}UdpPkgRecvHead;

typedef struct UdpPkgRecvBody
{
    RecvAction actions[0];
}UdpPkgRecvBody;

/*****************************/
typedef struct SendAction
{
    uint16_t vkey;
    uint16_t arg;
    uint8_t role_idx;
}SendAction;

typedef struct SendFrame
{
    uint16_t frame_id;
    uint8_t action_len;
    SendAction actions[0];
}SendFrame;

#endif
