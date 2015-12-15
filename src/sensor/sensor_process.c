/*
	Sensor process
 */
#include "base.h"
#include "zigbee/xbee_struct.h"
#include "sensor/sensor_process.h"
#include "sensor/sensor_client.h"
#include "sensor/sensor_struct.h"
#include "zigbee/xbee_client.h"
#include "drivers/semaphore.h"
#include <time.h>
#include <arpa/inet.h>


// Tableau des capteurs;
struct sensor * sensor_tab;

// Numéro du dernier sensor.
uint8_t num_sensor = 1;
int memoirePartagee;
int semaphore;


void sensor_init_process(void)
{
	
	// if ((memoirePartagee=shmget(CLEF,sizeof(struct sensor)*LIMIT_SENSOR,IPC_CREAT|0666)) == -1){
 //    	#ifdef __DEBUG__
	// 		printf("Impossible de creer le segment de memoire partagee");
	// 	#endif
		
 //  	} else {
 //  		sensor_tab =(struct sensor *)shmat(memoirePartagee,NULL,0);
 //  		#ifdef __DEBUG__
	// 		printf("Adresse de l'attachement : %p \n",sensor_tab);
	// 	#endif
 //  	}
  	sensor_tab = calloc(LIMIT_SENSOR, sizeof(struct sensor));
  	semaphore = init_sem();
	int i;
	for(i = 1; i < LIMIT_SENSOR; i++){
		sensor_tab[i].id = 0;
		sensor_tab[i].stat.nb_send = 0;
		sensor_tab[i].stat.nb_erreur_send = 0;
		sensor_tab[i].stat.nb_sucess = 0;
		sensor_tab[i].stat.nb_receive_total = 0;

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


void  set_init_refresh_time( uint8_t sensor_type, uint8_t * refresh_time)
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
			tmp_refresh_time[2] = 0x13;
			tmp_refresh_time[3] = 0xE8; 
		break;
		case SENSOR_LUMI :
		
			tmp_refresh_time[0] = 0;
			tmp_refresh_time[1] = 0;
			tmp_refresh_time[2] = 0x13;
			tmp_refresh_time[3] = 0xE8; 
		break;
		case SENSOR_AXIS :
		// 500 ms
			tmp_refresh_time[0] = 0;
			tmp_refresh_time[1] = 0;
			tmp_refresh_time[2] = 0x13;
			tmp_refresh_time[3] = 0xE8; 
		break;
		case SENSOR_ADC :
			tmp_refresh_time[0] = 0;
			tmp_refresh_time[1] = 0;
			tmp_refresh_time[2] = 0x13;
			tmp_refresh_time[3] = 0xE8;
			break;
		default:
			tmp_refresh_time[0] = 0;
			tmp_refresh_time[1] = 0;
			tmp_refresh_time[2] = 0;
			tmp_refresh_time[3] = 0;
		break;
	}

	memcpy(refresh_time, tmp_refresh_time, 4);


}

void set_data_len(uint16_t id, uint8_t type)
{
	
	
	if(type > NUM_TYPE_SENSOR+1){
		return ;
	}
	// if(!take_semaphore()){
	 	
	//     #ifdef __DEBUG__
	//       printf(" Impossible de prendre le semaphore ...\n");
	//     #endif
	//       return;
 //  	}

	switch(type){
		case SENSOR_TEMP :
			get_sensor_struct(id)->len_data = SENSOR_TEMP_LEN;
			get_sensor_struct(id)->n_data = 1;
		break;
		case SENSOR_LUMI :
			get_sensor_struct(id)->len_data = SENSOR_LUMI_LEN;
			get_sensor_struct(id)->n_data = 1;
		break;
		case SENSOR_AXIS :
			get_sensor_struct(id)->len_data = SENSOR_AXIS_LEN;
			get_sensor_struct(id)->n_data = 6;
		break;
		case SENSOR_ADC :
			get_sensor_struct(id)->len_data = SENSOR_ADC_LEN;
			get_sensor_struct(id)->n_data = 2;
		break;

		default:

		break;
	}
	// give_semaphore();
	 // if(!give_semaphore()){
  //   #ifdef __DEBUG__
  //     printf(" Impossible de rendre le semaphore ...\n");
  //   #endif
  //     return;
  // }
}

void update_ip(uint16_t id, uint8_t * ip)
{
	if(ip[0] != 0 && ip[1] != 0 && id < 50){
		memcpy(get_sensor_struct(id)->ip, ip, 2);
	}
	
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

void sensor_ask_id(uint8_t * data, uint16_t ip)
{
			uint8_t refresh_time[4];
			// while(!take_semaphore()){
			// 	printf("Prise du sémaphore ...\n");
			// }
			// On obtient un nouvelle ID.
			uint16_t id = sensor_get_new_id((uint8_t *)&ip);

			// On prend le sémaphore :
			while(take_sem_sensor() == -1){
				printf("Attente du semaphore\n");
			}

			// On note le type du capteur
			get_sensor_struct(id)->type = data[OFFSET_ASK+1];
			// On note ID de reconnaissance pour le FPGA.
			uint16_t id_fpga = data[SENSOR_IDH]<< 8 | data[SENSOR_IDL];


			// #ifdef __DEBUG__
			// printf(" --- Demande d'un nouvelle ID --- \n");
			// #endif
			uint8_t * data_to_send = (uint8_t *)calloc(GIVE_ID_LEN, sizeof(uint8_t));
			set_init_refresh_time(data[OFFSET_ASK + 1], refresh_time);
			set_data_len(id, data[OFFSET_ASK + 1]);
			//memcpy(refresh_time, set_refresh_time(data[OFFSET_ASK +1]), 4);
			memcpy(get_sensor_struct(id)->refresh_time, refresh_time, 4);
			
		// On a nos informations on informe le capteur de son ID et taux de rafraîchissement :
			data_to_send[SENSOR_IDH] = id>>8&0xFF;
			data_to_send[SENSOR_IDL] = id&0xFF;
			data_to_send[OFFSET_ASK] = GIVE_ID;
			data_to_send[OFFSET_ASK+1] = id_fpga>>8&0xFF;
			data_to_send[OFFSET_ASK+2] = id_fpga&0xFF;
			data_to_send[OFFSET_ASK+3] = get_sensor_struct(id)->type;
			memcpy(&data_to_send[OFFSET_ASK+4], refresh_time, 4);
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
			printf("\n");
			#endif
			// On rajoute l'envoie pour l'id :
			get_sensor_struct(id)->stat.nb_send++;
			give_sem_sensor();
			
			// xbee_send_data(data_to_send, GIVE_ID_LEN, 0, ip);
			xbee_insert_fifo(data_to_send,GIVE_ID_LEN);

			
}

void sensor_receive_id(uint8_t * data)
{
	// On récupère l'ID;
	uint16_t id = ((uint16_t)data[SENSOR_IDH] << 8 | data[SENSOR_IDL]);
	uint16_t id_fpga = ((uint16_t)data[OFFSET_ASK+1] << 8 | data[OFFSET_ASK+2]);
	#ifdef __DEBUG__
		printf(" ID reçu : %d\n", id);
	#endif
	// On parcours le tableau des sensor:
	int i;
	// if(!take_semaphore()){
	 	
	//     #ifdef __DEBUG__
	//       printf(" Impossible de prendre le semaphore ...\n");
	//     #endif
	//       return;
 //  	}
	for(i = 0; i < LIMIT_SENSOR; i++){
		if(get_sensor_struct(i)->id == id_fpga){
			get_sensor_struct(i)->id = id;
			memcpy(get_sensor_struct(i)->refresh_time, &data[OFFSET_ASK+3], 4);
			
			break;
		}
	}
	// give_semaphore();

}

void sensor_receive_data(uint8_t * data, uint16_t id, uint8_t len)
{
	uint8_t len_data, n_data, dec, i, j;
	int data_tmp;
	// printf("Donnée reçu :\n");
	// for(i =0; i < 8; i++){
	// 	printf("%x ", data[i]);
	// }
	// printf("\n");
	// On divise les données : 
	n_data = get_sensor_struct(id)->n_data;
	len_data = get_sensor_struct(id)->len_data;
	// TO DO : check len et len_data pour gestion des bug;
	for(i = 0; i < n_data*len_data; i = i + len_data){
		dec = 0;
		data_tmp = 0;

		for(j = 0; j < len_data; j++){
			// printf("data[OFFSET_ASK +1 + %d + %i] = %x \n", j,i, data[OFFSET_ASK +1 + j + i]);
			data_tmp  = (data_tmp<<dec) | (data[OFFSET_ASK +1 + j + i]);
			dec += 8;
		}
			get_sensor_struct(id)->data[i/len_data] = data_tmp;
			// printf("pour i = %d ; data = %d \n", i,data_tmp);
		
		//get_sensor_struct(id)->data[i];
	}

	//#ifdef __DEBUG__
	//	printf(" ---- Données reçu : ----\n");
	//	printf("De la part de : %d \n", id);
	//	printf("Valeur : ");

	//	for(i = 0; i < n_data; i++){
	//		printf("%d", get_sensor_struct(id)->data[i]);
	//	}
	//	printf("\n-----fin de données reçu ----\n");
	//#endif
}

void update_refresh_time(uint16_t id)
{
	uint32_t refresh_new = 	get_sensor_struct(id)->refresh_time[3]<< 24 |
							get_sensor_struct(id)->refresh_time[2]<< 16 |
							get_sensor_struct(id)->refresh_time[1]<< 8	|
							get_sensor_struct(id)->refresh_time[0];
	if(refresh_new != get_sensor_struct(id)->compar_refresh_time){
		// Le temps est différent on le transmet alors au FPGA pour qu'il le change.
		uint8_t data_to_send[UPDATE_REFRESH_LEN];
		data_to_send[SENSOR_IDH] = id>>8&0xFF;
		data_to_send[SENSOR_IDL] = id&0xFF;
		data_to_send[OFFSET_ASK] = UPDATE_REFRESH_LEN;
		memcpy(&data_to_send[OFFSET_ASK+1],get_sensor_struct(id)->refresh_time, 4);
		// xbee_send_data(data_to_send, UPDATE_REFRESH_LEN, 0, get_sensor_struct(id)->ip);
		xbee_insert_fifo(data_to_send,UPDATE_REFRESH_LEN);
		get_sensor_struct(id)->compar_refresh_time = refresh_new;

	}

}													



