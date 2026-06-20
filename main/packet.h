#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

typedef struct {
    uint8_t stx; //packet start marker. Value = 0xFD. Used to indicate the start of the new packet
    uint8_t length;  //Payload length. 0-255
    uint8_t incompat_flags; // Incompatiblity flags. flags that must be understood for mavlink compatibility
    uint8_t compat_flags; //compatiblity flags. flags that can be ignored if not understood.
    uint8_t seq; //Packet sequence number. 0-255. Used to detect packet loss.
    uint8_t sys_id;//System ID. 1-255. ID of the system sending the message.
    uint8_t comp_id; //COmponent ID.1-255. ID Of the component sending the message.
    uint32_t msg_id; //Message id. 0-16777215. ID of message type in payload;
    uint8_t payload[255]; //Payload. Message data
    uint16_t checksum; //checksum. 
   


} packet;


#endif