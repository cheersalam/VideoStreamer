#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "droneCommandHandler.h"
#include "config.h"
#include "jsmn.h"

typedef struct HANDSHAKE_DATA_T{
    int32_t status;
    int32_t c2d_port;
    int32_t arstream_fragment_size;
    int32_t arstream_fragment_maximum_number;
    int32_t arstream_max_ack_interval;
    int32_t c2d_update_port;
    int32_t c2d_user_port;
}HANDSHAKE_DATA_T;

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int32_t main(int32_t argc, char *argv[]) {
	int droneCommandfd = 0;;
	int32_t err = 0;
	char response[1024] = {0};
    HANDSHAKE_DATA_T handshakeData = {0};
	
	err = startNetwork(DRONE_IP_ADD, DRONE_COMM_PORT, &droneCommandfd);
    if (err) {
        printf("startNetwork Failed\n");
        return 0;
    }

	err = sendCommand(droneCommandfd, DRONE_HANDSHAKE_REQ, response, 1024 - 1);
    if(err) {
        printf("send command failed. Exiting\n");
        return 0;
    }
   
   parseHandshakeResponse(&response, &handshakeData);

}


int32_t parseHandshakeResponse(char *jsonStr, HANDSHAKE_DATA_T handshakeData) {
    int32_t     err = 0;
	int32_t     i = 0;
	int32_t     r = 0;
	jsmn_parser p;
	jsmntok_t   t[128]; /* We expect no more than 128 tokens */
	jsmn_init(&p);

    assert(jsonStr);

	r = jsmn_parse(&p, jsonStr, strlen(jsonStr), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", err);
		return err;
	}

    /* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(jsonStr, &t[i], "status") == 0) {
			/* We may use strndup() to fetch string value */
			printf("- Status: %.*s\n", t[i+1].end-t[i+1].start,
					jsonStr + t[i+1].start);
			i++;
		} else if (jsoneq(jsonStr, &t[i], "c2d_port") == 0) {
			/* We may additionally check if the value is either "true" or "false" */
			printf("- c2d_port: %.*s\n", t[i+1].end-t[i+1].start,
					jsonStr + t[i+1].start);
			i++;
		} else if (jsoneq(jsonStr, &t[i], "arstream_fragment_size") == 0) {
			/* We may want to do strtol() here to get numeric value */
			printf("- arstream_fragment_size: %.*s\n", t[i+1].end-t[i+1].start,
					jsonStr + t[i+1].start);
			i++;
		} else {
			printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
					jsonStr + t[i].start);
		}
	}


}

