/*
	Sensor client
 */
#include "base.h"
#include "drivers/semaphore.h"
#include "sensor/sensor_client.h"
#include "sensor/sensor_struct.h"
#include "sensor/sensor_process.h"

void sensor_init_client(void)
{
	sensor_init_process();

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
		// printf("ip = %d %d", ip[0], ip[1]);
		 update_ip(id,ip);
		 
		
		//uint16_t id = (uint16_t)data[0]<< 8 | data[1]; 
	switch(data[OFFSET_ASK]){
		#ifdef __BB__
		case ASK_NEW_ID:
				sensor_ask_id(data, ip);
				break;
		#endif
		#ifdef __FPGA__
		case GIVE_ID:
				sensor_receive_id(data);
			break;
		#endif
		case DATA: 
				sensor_receive_data(data, id, len);
				// get_sensor_struct(id)->stat.nb_receive_total++;
		break;
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
	xbee_send_data(data, ASK_DATA_LEN, 0, get_sensor_struct(id)->ip);
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


void frame_stat(struct tx_status * frame)
{
	// On récupère l'id et on note dans les stats :
	uint8_t id = frame->id;
	uint8_t i;
	for(i = 0; i < LIMIT_SENSOR; i++){
		if(i == id){
			// On mets a jour les stats :
			switch(frame->delivery_status){
			case 0x00:
				if(!take_sem_sensor()){
						//On a le sémaphore :
						get_sensor_struct(i)->stat.nb_sucess++;
						// printf("nb_sucess of %d : %d\n", i,get_sensor_struct(i)->stat.nb_sucess);
						give_sem_sensor();
					}
			break;
			default:
			if(!take_sem_sensor()){
					//On a le sémaphore :
					get_sensor_struct(i)->stat.nb_erreur_send++;

					give_sem_sensor();
				}
			break;
		}
			// On quite la boucle ;)
			break;
		}
	}

}