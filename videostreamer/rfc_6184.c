#include "rfc_6184.h"
#include "string.h"
#include "stdlib.h"
#include "utilities/utilities.h"

/*
 *  Payload Packet    Single NAL    Non-Interleaved    Interleaved
      Type    Type      Unit Mode           Mode             Mode
      -------------------------------------------------------------
      0      reserved      ig               ig               ig
      1-23   NAL unit     yes              yes               no
      24     STAP-A        no              yes               no
      25     STAP-B        no               no              yes
      26     MTAP16        no               no              yes
      27     MTAP24        no               no              yes
      28     FU-A          no              yes              yes
      29     FU-B          no               no              yes
      30-31  reserved      ig               ig               ig
 *
 *
 *
 */

static const unsigned char startCode[] = { 0, 0, 0, 1 };
static RTP_HEADER_T *parseRTPHeader(unsigned char *buffer, int32_t bufLen);

int32_t getBitByPos(unsigned char *buffer, int32_t pos) {
    return (buffer[pos/8] >> (8 - pos%8) & 0x01);
}


uint32_t decodeGolomb(unsigned char *byteStream, uint32_t *index) {
    uint32_t leadingZeroBits = -1;
    uint32_t codeNum = 0;
    uint32_t pos = *index;

    if (byteStream == NULL || pos == 0 ) {
        printf("Invalid input\n");
        return 0;
    }
    
    for (int32_t b = 0; !b; leadingZeroBits++)
        b = getBitByPos(byteStream, pos++);

    for (int32_t b = leadingZeroBits; b > 0; b--)
        codeNum = codeNum | (getBitByPos(byteStream, pos++) << (b - 1));
    
    *index = pos;
    return ((1 << leadingZeroBits) - 1 + codeNum);
}

int32_t allocateAndResetPacket(PACKET_T **packet) {
    PACKET_T *newPacket;
    newPacket = (PACKET_T *)malloc(sizeof(PACKET_T));
    if(newPacket == NULL) {
        packet = NULL;
        return -1;
    }
    newPacket->len = 0;
    newPacket->timestamp = 0;
    newPacket->seqNum = 0;
    newPacket->maxLen = 0;
    newPacket->buffer = NULL;
    *packet = newPacket;
    return 0;
}


void *initRFC6814() {
   RFC_6814_HANDLE_T *handle = (RFC_6814_HANDLE_T *)malloc(sizeof(RFC_6814_HANDLE_T));
   memset(handle, 0, sizeof(RFC_6814_HANDLE_T));

   if(0 != allocateAndResetPacket(&handle->packet[0])) {
       return NULL;
   }

   if(0 != allocateAndResetPacket(&handle->packet[1])) {
       return NULL;
   }

   return handle;
}

void closeRFC6814(void *handle) {
    RFC_6814_HANDLE_T *rfcHandle = handle;
   
}

int32_t rtpPacket(RFC_6814_HANDLE_T *rfcHandle, unsigned char *buffer, uint32_t bufLen, PACKET_T **nalPacket) {
    RTP_HEADER_T *rtpHeader = NULL;
    RFC_6814_HANDLE_T *handle = rfcHandle;
    uint32_t pos = 0;
    int32_t nalUnitType = 0;
    uint32_t decodeLen = 0;
    uint16_t nalUnitSize = 0;
    uint32_t first_mb_in_slice = 0;
    uint32_t nalSize = 0;
    uint32_t bufferInUse = 0;
    int32_t packetFinished = 0;
    uint32_t stapPacket = 1;
    void *newPacket = NULL;
    PACKET_T *packet;

    if(handle == NULL) {
        return -1;
    }

    packet = handle->packet[handle->packetIndex];
    rtpHeader = parseRTPHeader(buffer, bufLen);
    if(rtpHeader == NULL) {
        return -1;
    }
    
    pos = rtpHeader->hdrLen;
    while(pos != bufLen) {
        printf("pos = %d bufLen = %d\n", pos, bufLen);
        nalUnitType = buffer[pos] & 0x1F;
        if (nalUnitType == 1 && nalUnitType <= 23) {
            printf("Single NAL unit packet nalUnitType = %d \n", nalUnitType);

          }
        else if(nalUnitType == 24) { //STAP-A
        	pos++; //nal unit header | 2 bytes size | nal unit
            printf("STAP-A \n");
            if (!readShortToShort(buffer, bufLen, &pos, &nalSize)) {
            	stapPacket++;
                //nalUnitType = buffer[pos] & 0x1F;
                //pos += nalUnitSize;
                //printf("nalUnitType %d\n", nalUnitType);
            }
        }
        else if(nalUnitType == 25) { //STAP-B
            pos = bufLen;
            printf("STAP-B \n");
        }
        else if(nalUnitType == 26) { //MTAP16
            pos = bufLen;
            printf("MTAP16 \n");
        }
        else if(nalUnitType == 27) { //MTAP24
            pos = bufLen;
            printf("MTAP24 \n");
        }
        else if(nalUnitType == 28) { //FU-A
            pos = bufLen;
            printf("FU-A \n");
        }
        else if(nalUnitType == 29) { //FU-B
            pos = bufLen;
            printf("FU-B \n");
        }
        else {
        	pos = bufLen;
            printf("Reserved ... \n");
        }
    }
    return packetFinished;
}


static RTP_HEADER_T *parseRTPHeader(unsigned char *buffer, int32_t bufLen) {
	uint32_t pos = 0;
	int32_t err;
	uint16_t shortVal = 0;
	uint16_t seqNum = 0;
	RTP_HEADER_T *rtpHeader = NULL;;
	uint32_t timeStamp = 0;
	static int frameCount = 0;

    if(buffer == NULL || bufLen <= 16) {
        return NULL;
    }

	rtpHeader = (RTP_HEADER_T *)malloc(sizeof(struct RTP_HEADER_T));
    if(rtpHeader != NULL) {
        memset(rtpHeader, 0, sizeof(struct RTP_HEADER_T));

	    if (!readShortToShort(buffer, bufLen, &pos, &shortVal)) {
	       // printf("Short = %x pos = %d\n", shortVal, pos);
        }

	    if (!readShortToShort(buffer, bufLen, &pos, &seqNum)) {
            rtpHeader->seqNum = seqNum;
	       // printf("seqNum = %hu pos = %d\n", seqNum, pos);
        }

	    if(!read4Bytetoint32(buffer, bufLen, &pos, &timeStamp)) {
            rtpHeader->timestamp = timeStamp;
	        //printf("timestamp = %u pos = %d\n", timeStamp, pos);
        }
	    if(!read4Bytetoint32(buffer, bufLen, &pos, &timeStamp)) {
	    	rtpHeader->ssrc = timeStamp;
	    	//printf("ssrc = %u pos = %d\n", timeStamp, pos);
	    }
        rtpHeader->hdrLen = 12;
    }
    return rtpHeader;
}


