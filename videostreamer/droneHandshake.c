/*
 * Copyright © 2016 Mohammad Alam <cheersalam@gmail.com>
 */

#include "droneHandshake.h"
#include "tcpClientSocket.h"
#include "cJSON.h"

char *convertToJson(HANDSHAKE_REQ_T *handshakeRequest) {
	cJSON *root = NULL;
	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "d2c_port", handshakeRequest->d2c_port);
	cJSON_AddStringToObject(root, "controller_name", handshakeRequest->controller_name);
	cJSON_AddStringToObject(root, "controller_type", handshakeRequest->controller_type);
	cJSON_AddNumberToObject(root, "arstream2_client_stream_port", handshakeRequest->arstream2_client_stream_port);
	cJSON_AddNumberToObject(root, "arstream2_client_control_port", handshakeRequest->arstream2_client_control_port);
	char *ptr = cJSON_Print(root);
	cJSON_Delete(root);
	return ptr;
}

int32_t parseJsonToHandshakeData(char *jsonStr, HANDSHAKE_DATA_T *handshakeData) {
    cJSON *root = cJSON_Parse(jsonStr);
    if (NULL == root) {
        return -1;
    }
   
    handshakeData->status = cJSON_GetObjectItem(root, "status")->valueint;
    handshakeData->c2d_port = cJSON_GetObjectItem(root, "c2d_port")->valueint;
    handshakeData->arstream_fragment_size = cJSON_GetObjectItem(root, "arstream_fragment_size")->valueint;
    handshakeData->arstream_fragment_maximum_number = cJSON_GetObjectItem(root, "arstream_fragment_maximum_number")->valueint;
    handshakeData->arstream_max_ack_interval = cJSON_GetObjectItem(root, "arstream_max_ack_interval")->valueint;
    handshakeData->c2d_update_port = cJSON_GetObjectItem(root, "c2d_update_port")->valueint;
    handshakeData->c2d_user_port = cJSON_GetObjectItem(root, "c2d_user_port")->valueint;
    if ( NULL != cJSON_GetObjectItem(root, "arstream2_server_stream_port"))
       handshakeData->arstream2_server_stream_port = cJSON_GetObjectItem(root, "arstream2_server_stream_port")->valueint;
    if ( NULL != cJSON_GetObjectItem(root, "arstream2_server_control_port"))
       handshakeData->arstream2_server_control_port = cJSON_GetObjectItem(root, "arstream2_server_control_port")->valueint;
    if ( NULL != cJSON_GetObjectItem(root, "arstream2_max_packet_size"))
       handshakeData->arstream2_max_packet_size = cJSON_GetObjectItem(root, "arstream2_max_packet_size")->valueint;
    if ( NULL != cJSON_GetObjectItem(root, "arstream2_max_bitrate"))
       handshakeData->arstream2_max_bitrate = cJSON_GetObjectItem(root, "arstream2_max_bitrate")->valueint;

    cJSON_Delete(root);
    return 0;
}

void * handshakeWithdrone(char *droneIp, uint16_t dronePort, HANDSHAKE_DATA_T *handshakeData) {
	void *handshakeHandler = NULL;
	char *jsonStr = NULL;
	int32_t bytesSent = 0;
    char handshakeResponse[1024 * 1024] = {0};
    int32_t handshakeResLen = 0;
	HANDSHAKE_REQ_T handshakeRequest = { 0 };

    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__ );
	handshakeHandler = initTcpClientSocket(dronePort, droneIp, NULL);
	if (NULL == handshakeHandler) {
		return NULL;
	}

    printf("%s:%s:%d Sending Handshake request\n", __FILE__, __func__, __LINE__ );
	strncpy(handshakeRequest.controller_name, CONTROLLER_NAME, 64);
	strncpy(handshakeRequest.controller_type, CONTROLLER_TYPE, 64);
	handshakeRequest.d2c_port = D2C_PORT;
	handshakeRequest.arstream2_client_stream_port = 5004;
	handshakeRequest.arstream2_client_control_port = 5005;


	jsonStr = convertToJson(&handshakeRequest);
	if (NULL == jsonStr) {
		printf("Json parsing failed \n");
		closeTcpClient(handshakeHandler);
		return NULL;
	}

	bytesSent = sendSyncDataToTcpServer(handshakeHandler, jsonStr, strlen(jsonStr), handshakeResponse, &handshakeResLen);
	if (0 == bytesSent) {
		free(jsonStr);
		closeTcpClient(handshakeHandler);
		return NULL;
	}
    printf("handshake data %s\n", handshakeResponse);
    
    parseJsonToHandshakeData(handshakeResponse, handshakeData);
    free(jsonStr);
	return handshakeHandler;
}


