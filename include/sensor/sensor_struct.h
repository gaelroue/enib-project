#ifndef SENSOR_STRUCT
#define SENSOR_STRUCT


struct sensor
{
	uint16_t id;	/*< Numéro d'identification du capteur */
	uint8_t type; /*< Type du capteur (Température, luminosité (prédéfinis au préalable)*/
	int value_min; /*< Value minimal que le capteur peut transmettre. */
	int value_max; /*< Value maximal que le capteur peut transmettre. */
	uint8_t refresh_time; /*< 0 si c'est une demande de la BB ou 1 - 255 en secondes pour que le capteur renvois les infos. */
	int data; /*<  Donnée du capteur */
};

#endif