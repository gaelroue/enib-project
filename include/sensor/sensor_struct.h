#ifndef SENSOR_STRUCT
#define SENSOR_STRUCT

#include "base.h"

struct stats_sensor
{
	int state;
	#define OK		1
	#define NOK		2
	#define CHECK 	3
	int nb_send;
	int nb_receive_total;
	int nb_receive_update;
	int time_last_receive;
	int nb_erreur_receive;
	int nb_erreur_send;

	
};

struct sensor
{
	uint16_t id;	/*< Numéro d'identification du capteur */
	uint8_t type; /*< Type du capteur (Température, luminosité (prédéfinis au préalable)*/
	//int value_min; /*< Value minimal que le capteur peut transmettre. */
	//int value_max; /*< Value maximal que le capteur peut transmettre. */
	uint8_t refresh_time[4]; /*< 0 si c'est une demande de la BB ou 1 - 255 en secondes pour que le capteur renvois les infos. */
	uint8_t ip[2];
	uint8_t len_data;
	uint8_t n_data; /*< nombre de composante de donnée exemple :(x,y,z) gyroscope.*/
	#define N_DATA_MAX 	6
	int data[N_DATA_MAX]; /*<  Donnée du capteur */
	struct stats_sensor stat;
};






typedef struct
{
int id;
pthread_t th;
} ThreadData_t;
#endif