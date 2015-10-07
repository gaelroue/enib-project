#ifndef SENSOR_CLIENT
#define SENSOR_CLIENT

/**************************** 
* init_sensor_client - Initialise les paramètres des capteurs : 
* 			struct sensor_struct sensor_tab[MAX_SENSOR]  à 0. 
* 			next_id à 1.
*
*****************************/

void init_sensor_client(void);

/****************************
* new_sensor - Permet de définir un nouveau capteur. 
* in :	type - uint8_t  
*		data_type - uint8_t
*		value_min - uint8_t
*		value_max - uint8_t
* return : id - uint8_t 
*****************************/
uint8_t new_sensor(uint8_t type, uint8_t data_type, uint8_t value_min, uint8_t value_max);

/****************************
* remove_sensor - permet de supprimer un capteur.
* in : id - uint8_t
* return : errno - int
****************************/
int remove_sensor(uint8_t id);

/***************************
* rawdata_sensor - Traite une donnée venu du Xbee.
* in : data - * uint8_t 
* return errno - int 
******************************/
int rawdata_sensor(uint8_t * data);


#endif