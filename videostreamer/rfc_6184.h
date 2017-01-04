#include <stdio.h>
#include <stdint.h>

typedef struct RTP_HEADER_T{
	int32_t markerBit;
    int32_t csrcCount;
    int32_t payloadType;
    uint32_t seqNum;
    uint32_t timestamp;
    int32_t ssrc;
    int32_t csrc[16];
    int32_t hdrLen;
}RTP_HEADER_T;

typedef struct PACKET_T {
    uint32_t timestamp;
    uint32_t seqNum;
    unsigned char *buffer;
    size_t len;
    size_t maxLen;
}PACKET_T;

typedef struct RFC_6814_HANDLE_T {
    PACKET_T *packet[2];
    uint32_t packetIndex;
    uint32_t firstPacketPos;
    uint32_t lastSeqNum;
    uint32_t lastTimestamp;
}RFC_6814_HANDLE_T;


int32_t rtpPacket(RFC_6814_HANDLE_T *handle, unsigned char *buffer, uint32_t bufLen, PACKET_T **nalPacket);

