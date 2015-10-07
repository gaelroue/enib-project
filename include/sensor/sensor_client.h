#ifndef SENSOR_CLIENT
#define SENSOR_CLIENT

#define OFFSET_ASK			0x02
// Code d'identification de trame
#define ASK_NEW_ID 			0xFE
#define ASK_GIVE_ID			0xFD
#define ASK_SENSOR_INFORM	0xFC
#define ASK_ACK				0xFB
#define ASK_DATA			0x01


/**************************** 
* sensor_init_client - Initialise les paramètres des capteurs : 
* 			struct sensor_struct sensor_tab[MAX_SENSOR]  à 0. 
* 			next_id à 1.
*
*****************************/

void sensor_init_client(void);

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
* in : 	data - * uint8_t 
		len - uint8_t
* return errno - int 
******************************/
int rawdata_sensor(uint8_t * data, uint8_t len);


#endif