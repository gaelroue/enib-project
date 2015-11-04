/*
	Sensor client
 */
#include "base.h"
#include "sensor/sensor_client.h"
#include "sensor/sensor_struct.h"
#include "sensor/sensor_process.h"
#include "drivers/semaphore.h"




void sensor_init_client(void)
{
	sensor_init_process();
	init_semaphore();
}
void sensor_close_client(void)
{
	remove_shm();
}

uint8_t new_sensor(uint8_t type, uint8_t data_type, uint8_t value_min, uint8_t value_max){
	return 0;
}


int remove_sensor(uint16_t id){
	// supression du sensor : 
	get_sensor_struct(id)->id = 0;
	get_sensor_struct(id)->type = 0;
	//get_sensor_struct(id)->refresh_time = calloc(0, 4* sizeof(uint8_t));
	//get_sensor_struct(id)->ip = calloc(0, 2* sizeof(uint8_t));
	get_sensor_struct(id)->len_data = 0;
	get_sensor_struct(id)->n_data = 0;

}


int rawdata_sensor(uint8_t * data, uint8_t len, uint8_t * ip)
{
	// On check le premier élement pour rediriger :
	// cf : connexion_capteur.odt
	/*
		exemples de data :
		 00 00  FE 	01
		(  ID ) ASK
		 00 0C  FC  01 01 01 03 02 01 FF FF

	*/
		 uint16_t id = data[SENSOR_IDH]<< 8 | data[SENSOR_IDL];
		 update_ip(id,ip);

		 uint8_t refresh_time[4];
		 uint8_t len_data = 0;
		 uint8_t i,n_data,j;
		 uint8_t dec = 0;
		 int data_tmp;
		//uint16_t id = (uint16_t)data[0]<< 8 | data[1]; 
	switch(data[OFFSET_ASK]){
		case ASK_NEW_ID:
			id = sensor_get_new_id(ip);
			get_sensor_struct(id)->type = data[OFFSET_ASK+1];
			#ifdef __DEBUG__
			printf(" --- Demande d'un nouvelle ID --- \n");
			#endif
			data = calloc(ASK_GIVE_ID_LEN, sizeof(uint8_t));
			set_refresh_time(data[OFFSET_ASK + 1], refresh_time);
			set_data_len(id, data[OFFSET_ASK + 1]);
			//memcpy(refresh_time, set_refresh_time(data[OFFSET_ASK +1]), 4);
			memcpy(get_sensor_struct(id)->refresh_time, refresh_time, 4);
			
		// On a nos informations on informe le capteur de son ID et taux de rafraîchissement :
			data[SENSOR_IDH] = id>>8&0xFF;
			data[SENSOR_IDL] = id&0xFF;
			data[OFFSET_ASK] = ASK_GIVE_ID;
			memcpy(&data[OFFSET_ASK+1], refresh_time, 4);
			// ***************** SEND ************************
			#ifdef __DEBUG__
			printf("Envoie de l'ID et du taux de rafraîchissement au capteur \n");
			printf("ID : %d\n", id); 
			uint32_t debug_refresh = refresh_time[0]<<24 |
									 refresh_time[1]<<16 |
									 refresh_time[2]<<8  |
									 refresh_time[3]&0xff;
			printf("Refresh : %d\n", debug_refresh);
			printf("Type de capteur : %d \n", get_sensor_struct(id)->type);
			printf("--- END nouvelle ID ---\n");

			#endif
			//xbee_send_data(data, ASK_GIVE_ID_LEN, 0, ip);
			break;
		case DATA: 

			// On divise les données : 
			n_data = get_sensor_struct(id)->n_data;
			len_data = get_sensor_struct(id)->len_data;
			for(i = 0; i < n_data; i++){
				dec = 0;
				data_tmp = 0;
				for(j = 0; j < len_data; j++){

					data_tmp  = (data_tmp<<dec) | (data[OFFSET_ASK+1 + j]);
					dec = 8;
				}
 				get_sensor_struct(id)->data[i] = data_tmp;
				
				//get_sensor_struct(id)->data[i];
			}
			#ifdef __DEBUG__
				printf(" ---- Données reçu : ----\n");
				printf("De la part de : %d \n", id);
				printf("Valeur : ");

				for(i = 0; i < n_data; i++){
					printf("%d", get_sensor_struct(id)->data[i]);
				}
				printf("\n-----fin de données reçu ----\n");
			#endif

		break;

/*
		case ASK_ACK :
			// TO DO : send ack or nack...
		break;
		case ASK_DATA:
			// On copie les données dans la data :
			// TO DO : gérer les erreurs.
			sensor_copy_data(data);
		break;
		*/
		default:
		break;
	}

	return 0;
}

uint8_t get_data_from_sensor(uint16_t id)
{
	uint8_t * data = (uint8_t *)calloc(ASK_DATA_LEN, sizeof(uint8_t));
	data[SENSOR_IDH] = id >> 8 & 0xFF;
	data[SENSOR_IDL] = id & 0xFF;
	data[OFFSET_ASK] = ASK_DATA;
	#ifdef __DEBUG__
		printf(" Demande de données pour le capteur n°%d \n", id );
		printf("data : ");
		uint8_t i;
		for(i = 0; i < ASK_DATA_LEN; i++){
			printf(" %x", data[i]);
		}
		printf("\n");
	#endif
	//xbee_send_data(data, ASK_DATA_LEN, 0, get_sensor_struct(id)->ip);
}

#ifdef __FPGA__
uint8_t set_sensor_fpga(uint8_t * sensor, uint8_t nsensor){
	uint8_t i = 0;
	uint8_t data[4];
	for(i = 0; i< nsensor; i++){
		// On demande un ID et on attend la réponse... 
		data[0] = 0;
		data[1] = 0;
		data[2] = 0xFE;
		data[2] = sensor[i]; // type du sensor.
		//xbee_send_data(data)

	}

	return 0;
}
#endif