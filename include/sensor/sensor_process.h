#ifndef SENSOR_PROCESS
#define SENSOR_PROCESS


#define LIMIT_SENSOR		50
#define SENSOR_IDH			0
#define SENSOR_IDL			1
#define SENSOR_OPTION_BEGIN 3

/***** OPTION TYPE *****/ 
#define OPTION_SENSOR_TYPE		0x01
#define OPTION_VALUE_MIN		0x02
#define OPTION_VALUE_MAX		0x03
#define OPTION_END				0xFF



/*****************************
* sensor_init_process
* Initialise le process des capteurs.
******************************/
void sensor_init_process(void);
/*****************************
* sensor_get_new_id
* return : new_id - uint16_t
*******************************/
uint16_t sensor_get_new_id(void);
/*****************************
*  sensor_parse_inform
* in : data  - uint8_t *
* Permet de parser le capteur et de remplir sa structure.
***********************************/
int sensor_parse_inform(uint8_t * data, uint16_t len);
/*****************************
* sensor_copy_data
* in : data - * uint8_t 
***********************************/
int sensor_copy_data(uint8_t * data);


uint16_t id_sensor(uint8_t *data);

// ERREUR défine.
#define ERR_ID_PARSE 		-2 /*< l'ID du capteur ne correspond pas à ce que l'on attend (LIMIT_SENSOR).*/
#endif