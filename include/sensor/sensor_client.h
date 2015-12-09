#ifndef SENSOR_CLIENT
#define SENSOR_CLIENT

#define OFFSET_ASK			0x02
// Code d'identification de trame
#define ASK_NEW_ID 			0xFE /*< Demande de nouvelle ID */
#define GIVE_ID			0xFD /*< Donne l'ID accompagner du taux de rafraîchissement*/
	#define GIVE_ID_LEN 0x0A
#define DATA				0xFC /*< Transfert de donnée*/

#define UPDATE_REFRESH		0xFA
		#define UPDATE_REFRESH_LEN 0x07
#define ASK_DATA			0x02 /*< Demande de donnée non périodique*/
	#define ASK_DATA_LEN		0x03
#define ASK_ALIVE			0x22 /*< Gestion d'erreur, cf si le distant est vivant*/
#define START_COMMUNICATION 0xFB /*< Debut d'une communication entre BB et fpga */


/**************************** 
* sensor_init_client - Initialise les paramètres des capteurs : 
* 			struct sensor_struct sensor_tab[MAX_SENSOR]  à 0. 
* 			next_id à 1.
*
*****************************/

void sensor_init_client(void);
void sensor_close_client(void);
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
int remove_sensor(uint16_t id);

/***************************
* rawdata_sensor - Traite une donnée venu du Xbee.
* in : 	data - * uint8_t 
		len - uint8_t
* return errno - int 
******************************/
int rawdata_sensor(uint8_t * data, uint8_t len, uint8_t * ip);




#ifdef __FPGA__
uint8_t set_sensor_fpga(uint8_t * sensor, uint8_t nsensor);
#endif


#endif