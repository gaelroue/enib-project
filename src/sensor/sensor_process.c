/*
	Sensor process
 */
#include "base.h"
#include "sensor/sensor_process.h"
#include "sensor/sensor_struct.h"
#include <arpa/inet.h>

// Tableau des capteurs;
struct sensor * sensor_tab;

// Numéro du dernier sensor.
uint8_t num_sensor = 1;


void sensor_init_process(void)
{
	//uint8_t i;
	//for( i = 0; i< LIMIT_SENSOR; i++){
		sensor_tab = calloc(LIMIT_SENSOR, sizeof(struct sensor));
	//}
}

uint16_t sensor_get_new_id(uint16_t ip)
{
	uint8_t i;
	if(num_sensor >= LIMIT_SENSOR){
		num_sensor = 1;
	}
	for(i = 0; i < LIMIT_SENSOR; i++){
		if(sensor_tab[i].id ==  0){
			sensor_tab[i].id = num_sensor++;
			sensor_tab[i].ip = ip;
			break;
		}
	}

	return sensor_tab[i].id;
}


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