/*
	Sensor client
 */
#include "base.h"
#include "sensor/sensor_client.h"
#include "sensor/sensor_struct.h"
#include "sensor/sensor_process.h"
#include "zigbee/xbee_client.h"




void sensor_init_client(void){
	sensor_init_process();
}

uint8_t new_sensor(uint8_t type, uint8_t data_type, uint8_t value_min, uint8_t value_max){
	return 0;
}


int remove_sensor(uint8_t id){
	return -1;
}


int rawdata_sensor(uint8_t * data, uint8_t len,uint16_t ip){
	// On check le premier élement pour rediriger :
	// cf : connexion_capteur.odt
	/*
		exemples de data :
		 00 00  FE 
		(  ID ) ASK
		 00 0C  FC  01 01 01 03 02 01 FF FF

	*/
		 uint8_t * data_to_send;
		 int len_to_send;
		 uint16_t new_id;
		//uint16_t id = (uint16_t)data[0]<< 8 | data[1]; 
	switch(data[OFFSET_ASK]){
		case ASK_NEW_ID:
			 new_id = sensor_get_new_id(ip);
			printf("Demande d'un nouvelle ID  : %d \n", new_id);
			len_to_send = 3;
			data_to_send = malloc(len_to_send);
			data_to_send[0] = (uint16_t)new_id<<8&0xFF00;
			data_to_send[1] = new_id&0xFF;
			data_to_send[2] = ASK_GIVE_ID;
			xbee_send_data(data_to_send,len_to_send,0,0);
			#ifdef __DEBUG__
				printf("Envoie effectué...\n");
			#endif
			// TO DO : inform stats that a new send has been made.
			break;
		case ASK_GIVE_ID: 
			// TO DO : see for how to save the ID to one sensor in FPGA
			#ifdef __DEBUG__
				printf("Nouvelle ID reçu...\n");
			#endif

		break;
		case ASK_SENSOR_INFORM:
			//printf("Sensor Inform\n");
		// TO DO : gérer les erreurs.
			sensor_parse_inform(data,len);
			break;
		case ASK_ACK :
			// TO DO : send ack or nack...
		break;
		case ASK_DATA:
			// On copie les données dans la data :
			// TO DO : gérer les erreurs.
			sensor_copy_data(data);
		break;
		default:
		break;
	}

	return 0;
}