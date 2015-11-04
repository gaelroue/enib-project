/*
	Sensor process
 */
#include "base.h"
#include "sensor/sensor_process.h"
#include "sensor/sensor_client.h"
#include "sensor/sensor_struct.h"
#include "zigbee/xbee_client.h"
#include <time.h>
#include <arpa/inet.h>


// Tableau des capteurs;
struct sensor * sensor_tab;

// Numéro du dernier sensor.
uint8_t num_sensor = 1;
int memoirePartagee;

void sensor_init_process(void)
{
	
	if ((memoirePartagee=shmget(CLEF,sizeof(struct sensor)*LIMIT_SENSOR,IPC_CREAT|0666)) == -1){
    	#ifdef __DEBUG__
			printf("Impossible de creer le segment de memoire partagee");
		#endif
		sensor_tab = calloc(LIMIT_SENSOR, sizeof(struct sensor));
  	} else {
  		sensor_tab =(struct sensor *)shmat(memoirePartagee,NULL,0);
		printf("Adresse de l'attachement : %p \n",sensor_tab);
  	}

}

void remove_shm(void)
{
	shmctl(memoirePartagee,IPC_RMID,0);
}


uint16_t sensor_get_new_id(uint8_t * ip)
{
	uint8_t i;
	if(num_sensor >= LIMIT_SENSOR){
		num_sensor = 1;
	}
	for(i = 1; i < LIMIT_SENSOR; i++){
		if(sensor_tab[i].id ==  0){
			sensor_tab[i].id = num_sensor++;
			memcpy(sensor_tab[i].ip, ip, 2);
			break;
		}
	}

	return sensor_tab[i].id;
}

struct sensor * get_sensor_struct(uint16_t id){
	return &sensor_tab[id];
}
/*
int sensor_parse_inform(uint8_t * data, uint16_t len)
{
	uint16_t id = id_sensor(data);

	uint8_t * end = data + len;
	uint8_t * optptr = &data[SENSOR_OPTION_BEGIN];
  	uint8_t type = 0;

	while(optptr < end) {
	    switch(*optptr) {
	    case OPTION_SENSOR_TYPE:

	      memcpy(&(sensor_tab[id].type), optptr + 2, 1);
	       #ifdef __DEBUG__ 
	    	printf(" Sensor type : ");
	      	printf("%d\n", sensor_tab[id].type);
	      #endif
	      break;
	    case OPTION_VALUE_MIN:
	    // TO DO : faire une fonction comme memcpy mais qui gère les endians
	      memcpy(&(sensor_tab[id].value_min), optptr + 2, optptr[1]);
	      #ifdef __DEBUG__ 
	    	printf(" Sensor Minimal : ");
	      	printf("%x\n", sensor_tab[id].value_min);
	      #endif
	      break;
	    case OPTION_VALUE_MAX:
	      memcpy(&(sensor_tab[id].value_max), optptr + 2, optptr[1]);
	      #ifdef __DEBUG__ 
	    	printf(" Sensor Maximal : ");
	      	printf("%x\n", sensor_tab[id].value_max);
	      #endif
	      break;
	    case OPTION_END:
	      //return type;
	      break;
	    }

	    optptr += optptr[1] + 2;

	}
}
*/

int sensor_copy_data(uint8_t * data){
		// On récupère l'ID du capteur:
	uint16_t id = id_sensor(data);
	memcpy(&(sensor_tab[id].data), &data[4], data[3]);
	#ifdef __DEBUG__
		printf(" Valeur reçu : %d\n", sensor_tab[id].data);
	#endif
	return 0;
}

uint16_t id_sensor(uint8_t *data){
		// On récupère l'ID du capteur:
	uint16_t id = (uint16_t)(data[SENSOR_IDH]<< 8) | data[SENSOR_IDL];
	// On vérifie que l'ID est correct : 
	if(id >  LIMIT_SENSOR ){
		#ifdef __DEBUG__
		printf("Erreur de l'ID : %d \n", id);
	#endif
		//return ERR_ID_PARSE;
	}
	#ifdef __DEBUG__
		printf("ID du capteur : %d \n", id);
	#endif

	return id;
}


void  set_refresh_time( uint8_t sensor_type, uint8_t * refresh_time)
{
	// En fonction du sensor_type nous aurons un taux de rafraichissement par défault.
	uint8_t tmp_refresh_time[4] = {0, 0, 0, 0};
	if(sensor_type > NUM_TYPE_SENSOR+1){
		return ;
	}

	switch(sensor_type){
		case SENSOR_TEMP :
		// 1 seconde;
			tmp_refresh_time[0] = 0;
			tmp_refresh_time[1] = 0;
			tmp_refresh_time[2] = 0x03;
			tmp_refresh_time[3] = 0xE8; 
		break;
		case SENSOR_LUMI :
		
			tmp_refresh_time[0] = 0;
			tmp_refresh_time[1] = 0;
			tmp_refresh_time[2] = 0;
			tmp_refresh_time[3] = 0; 
		break;
		case SENSOR_PRESS :
		// 2 seconde
			tmp_refresh_time[0] = 0;
			tmp_refresh_time[1] = 0;
			tmp_refresh_time[2] = 0x07;
			tmp_refresh_time[3] = 0xD0; 
		break;
		default:

		break;
	}

	memcpy(refresh_time, tmp_refresh_time, 4);


}

void set_data_len(uint16_t id, uint8_t type)
{
	
	
	if(type > NUM_TYPE_SENSOR+1){
		return ;
	}

	switch(type){
		case SENSOR_TEMP :
			get_sensor_struct(id)->len_data = SENSOR_TEMP_LEN;
			get_sensor_struct(id)->n_data = 1;
		break;
		case SENSOR_LUMI :
			get_sensor_struct(id)->len_data = SENSOR_LUMI_LEN;
			get_sensor_struct(id)->n_data = 1;
		break;
		case SENSOR_PRESS :
			get_sensor_struct(id)->len_data = SENSOR_PRESS_LEN;
			get_sensor_struct(id)->n_data = 1;
		break;
		default:

		break;
	}
}

void update_ip(uint16_t id, uint8_t * ip)
{

	memcpy(sensor_tab[id].ip, ip, 2);
}

void  * beagle_check_sensor(void * data)
{
	ThreadData_t *td=(ThreadData_t *)data;
	uint8_t i = 0;
	useconds_t time_to_wait = 100000; /*< en µseconde */
	while(1){
		// On envoi :
		uint8_t data[3];
		data[SENSOR_IDH] = td->id >> 8; 
		data[SENSOR_IDL] = td->id & 0xFF;
		data[OFFSET_ASK] = 0xFB;

		uint16_t ip = (get_sensor_struct(td->id)->ip[1] << 8) | get_sensor_struct(td->id)->ip[0];
		
		#ifdef __DEBUG__
			printf(" Envoie de la donnée n°%d...\n", i++);
		#endif
			// xbee_send_data(data, 3, 0, ip);

			usleep(time_to_wait);
			if(get_sensor_struct(td->id)->stat.state == OK && i < 5){
				#ifdef __DEBUG__
					printf("Sensor come back !\n");
				#endif

				break;
			}else if( i >= 5){
				#ifdef __DEBUG__
					printf("Sensor loose sorry guy !\n");
				#endif
				remove_sensor(td->id);
				break;
			}
		
			
			time_to_wait = time_to_wait + time_to_wait;
			
	}
}