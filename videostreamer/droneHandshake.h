/*
 * Copyright © 2016 Mohammad Alam <cheersalam@gmail.com>
 */

#ifndef DRONE_HANDSHAKE_H
#define DRONE_HANDSHAKE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#define CONTROLLER_NAME "toto"
#define CONTROLLER_TYPE "tata"
#define D2C_PORT 43210

typedef struct HANDSHAKE_DATA_T {
	int32_t status;
	int32_t c2d_port;
	int32_t arstream_fragment_size;
	int32_t arstream_fragment_maximum_number;
	int32_t arstream_max_ack_interval;
	int32_t c2d_update_port;
	int32_t c2d_user_port;
	int32_t arstream2_server_stream_port;
	int32_t arstream2_server_control_port;
	int32_t arstream2_max_packet_size;
	int32_t arstream2_max_latency;
	int32_t arstream2_max_network_latency;
	int32_t arstream2_max_bitrate;
	int32_t arstream2_parameter_sets;
}HANDSHAKE_DATA_T;

typedef struct HANDSHAKE_REQ_T {
	uint16_t d2c_port;
	char controller_name[64];
	char controller_type[64];
	uint16_t arstream2_client_stream_port;
	uint16_t arstream2_client_control_port;
}HANDSHAKE_REQ_T;

void *handshakeWithdrone(char *droneIp, uint16_t dronePort, HANDSHAKE_DATA_T *handshakeData);

int32_t startNetwork(const char *droneIpAdd, uint16_t port, int *droneCommandfd);
int32_t startHandshake(int sockfd, const char *command, char *responseBuf, ssize_t responseBufLen);

#endif //DRONE_HANDSHAKE_H
