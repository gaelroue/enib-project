/*
	Sensor client
 */
#include "base.h"
#include "sensor/sensor_client.h"
#include "sensor/sensor_struct.h"
#include "sensor/sensor_process.h"




void sensor_init_client(void){
	sensor_init_process();
}

uint8_t new_sensor(uint8_t type, uint8_t data_type, uint8_t value_min, uint8_t value_max){
	return 0;
}


int remove_sensor(uint8_t id){
	return -1;
}


int rawdata_sensor(uint8_t * data, uint8_t len){
	// On check le premier élement pour rediriger :
	// cf : connexion_capteur.odt
	/*
		exemples de data :
		 00 00  FE 
		(  ID ) ASK
		 00 0C  FC  01 01 01 03 02 01 FF FF

	*/
		 uint16_t new_id;
		//uint16_t id = (uint16_t)data[0]<< 8 | data[1]; 
	switch(data[OFFSET_ASK]){
		case ASK_NEW_ID:
			 new_id = sensor_get_new_id();
			printf("Demande d'un nouvelle ID : %d \n", new_id);
		case ASK_GIVE_ID: 
			// TODO : Retourner alors l'ID au capteur.
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