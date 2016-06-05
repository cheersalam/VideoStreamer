#include <stdio.h>
#include <stdint.h>
#include "droneCommandHandler.h"
#include "config.h"

int32_t main(int32_t argc, char *argv[]) {
	int droneCommandfd = 0;;
	int32_t err = 0;
	char response[1024] = {0};
	
	err = startNetwork(DRONE_IP_ADD, DRONE_COMM_PORT, &droneCommandfd);
	if (!err) {
		sendCommand(droneCommandfd, DRONE_HANDSHAKE_REQ, response, 1024 - 1);
	}
}
