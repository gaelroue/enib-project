#ifndef DEFS_H
#define DEFS_H 1

/** Typedefs : **/
typedef unsigned char 		uint8_t;
typedef unsigned short 		uint16_t;
typedef unsigned int		uint32_t;

/* offset pour retrouver le checksum
EXEMPLE :
7E 00 04 08 01 41 41 74
 0  1  2  3  4  5  6  7
 		  3+len(4)	  CK	  
*/
#define OFFSET_CHECKSUM		3

int send_uart(int len, uint8_t * frame);
void chksum(uint8_t * frame);


struct Xbee_h
{
	uint8_t delimiter; /*< Début du paquet; toujours égal à 0x7E */
		// TO DO : cf pour htons/htonl
	uint16_t len;/*< Longueur de la trame entre len et checksum.*/
	uint8_t api;/*< Api mode */
	uint8_t frameID; /*< Id de la frame (TO DO : cf si on peut l'utiliser comme ID capteur )*/
};

struct AT_commande
{
	uint8_t at_cmd[2]; /*< Nom de commande en ASCII */
	uint8_t *at_cmddata;/*< pointeur sur le premier elèment de donnée; */
};

struct Tx_request
{
	uint8_t destmac[8]; /*< Adresse MAC de destination */
	uint16_t destaddr; /*< Adresse dynamique sur le réseau*/
	uint8_t radius; /*< Sets the maximum number of hops for a broadcast transmission. If zero, the network maximum hops value will be used.*/
	uint8_t options /*<Transmit options bitfield.
Unused bits must be set to 0.
0x01 - Disable retries and route repair
0x20 - Enable APS encryption (if EE=1) Enabling APS encryption decreases the maximum number of RF payload bytes by 4 (below the value reported by NP).
0x40 - Use the extended transmission timeout for this destination. Setting the extended timeout bit causes the stack to set the extended transmission timeout for the destination address.*/
	uint8_t demande_data; /*< Type de donnée; cf connexion_capteur.odt*/
	uint8_t * data; /*< pointeur sur le premier élément des données */

};

struct remoteAT_commande
{
	uint8_t destmac[8]; /*< Adresse MAC de destination */
	uint16_t destaddr; /*< Adresse dynamique sur le réseau*/
	uint8_t cmdOptions;/*<0x02 - Apply changes on remote device.
NOTE: If this bit is not set, an AC (or WR+FR) command must be sent before changes will take effect.
All other bits must be set to zero.*/
	uint8_t at_cmd[2]; /*/*< Nom de commande en ASCII */
	uint8_t *at_cmddata;/*< pointeur sur le premier elèment de donnée; */
};
#endif