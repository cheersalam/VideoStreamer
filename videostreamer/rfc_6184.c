#include "rfc_6184.h"
#include "string.h"
#include "stdlib.h"
#include "utilities/utilities.h"

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

void *initRFC6814() {
    return malloc(sizeof(RFC_6814_HANDLE_T));
}

void closeRFC6814(void *handle) {
    RFC_6814_HANDLE_T *rfcHandle = handle;
   
}

int32_t rtpPacket(unsigned char *buffer, uint32_t bufLen, unsigned char *nalPacket, uint32_t nalPacketLen) {
    RTP_HEADER_T *rtpHeader = NULL;
    uint32_t pos = 0;
    int32_t nalUnitType = 0;
    uint16_t nalUnitSize = 0;

    rtpHeader = parseRTPHeader(buffer, bufLen);
    if(rtpHeader == NULL) {
        return -1;
    }
    
    pos = rtpHeader->hdrLen;
    while(pos != bufLen) {
        nalUnitType = buffer[pos] & 0x1F;

        if (nalUnitType == 1 && nalUnitType <= 23) {
            printf("Single NAL unit packet nalUnitType = %d \n", nalUnitType);
        }
        else if(nalUnitType == 24) { //STAP-A
            printf("STAP-A \n");
            if (!readShortToShort(buffer, bufLen, &pos, &nalUnitSize)) {
                nalUnitType = buffer[pos] & 0x1F;
                printf("nalUnitType %d\n", nalUnitType);
            }
        }
        else if(nalUnitType == 25) { //STAP-B
            printf("STAP-B \n");
        }
        else if(nalUnitType == 26) { //MTAP16
            printf("MTAP16 \n");
        }
        else if(nalUnitType == 27) { //MTAP24
            printf("MTAP24 \n");
        }
        else if(nalUnitType == 28) { //FU-A
            printf("FU-A \n");
        }
        else if(nalUnitType == 29) { //FU-B
            printf("FU-B \n");
        }
        else {
            printf("Reserved ... \n");
        }
    }
    return 0;
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
	        printf("Short = %x pos = %d\n", shortVal, pos);
        }

	    if (!readShortToShort(buffer, bufLen, &pos, &seqNum)) {
            rtpHeader->seqNum = seqNum;
	        printf("seqNum = %hu pos = %d\n", seqNum, pos);
        }

	    if(!read4Bytetoint32(buffer, bufLen, &pos, &timeStamp)) {
            rtpHeader->timestamp = timeStamp;
	        printf("timestamp = %u pos = %d\n", timeStamp, pos);
        }
    }
    return rtpHeader;
}


