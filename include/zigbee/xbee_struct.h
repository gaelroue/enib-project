#ifndef XBEE_STRUCT
#define XBEE_STRUCT

struct __attribute__((__packed__)) xbee_header
{
	uint8_t delimiter; /*< Début du paquet; toujours égal à 0x7E */
		// TO DO : cf pour htons/htonl
	uint16_t length;/*< Longueur de la trame entre len et checksum.*/
	uint8_t api;/*< Api mode */
};

struct __attribute__((__packed__)) xbee_rawframe
{
	struct xbee_header header;
	uint8_t rawdata[];
};

struct __attribute__((__packed__)) at_command
{
	uint16_t at_cmd; /*< Nom de commande en ASCII */
	uint8_t data_at_cmd[];/*< pointeur sur le premier elèment de donnée; */
};

struct __attribute__((__packed__)) xbee_atcommand
{
	struct xbee_header header;
	uint8_t frame_id; /*< Id de la frame (TO DO : cf si on peut l'utiliser comme ID capteur )*/
	struct at_command at;
};

struct __attribute__((__packed__)) tx_request
{
	uint8_t dest_mac[8]; /*< Adresse MAC de destination */
	uint16_t dest_addr; /*< Adresse dynamique sur le réseau*/
	uint8_t radius; /*< Sets the maximum number of hops for a broadcast transmission. If zero, the network maximum hops value will be used.*/
	uint8_t options; /*<Transmit options bitfield.
Unused bits must be set to 0.
0x01 - Disable retries and route repair
0x20 - Enable APS encryption (if EE=1) Enabling APS encryption decreases the maximum number of RF payload bytes by 4 (below the value reported by NP).
0x40 - Use the extended transmission timeout for this destination. Setting the extended timeout bit causes the stack to set the extended transmission timeout for the destination address.*/
	uint8_t data[]; /*< pointeur sur le premier élément des données */
};

struct __attribute__((__packed__)) xbee_dataframe
{
	struct xbee_header header;
	uint8_t frame_id; /*< Id de la frame (TO DO : cf si on peut l'utiliser comme ID capteur )*/
	struct tx_request tx;

};

struct __attribute__((__packed__)) remote_at_command
{
	uint8_t dest_mac[8]; /*< Adresse MAC de destination */
	uint16_t dest_addr; /*< Adresse dynamique sur le réseau*/
	uint8_t cmd_options;/*<0x02 - Apply changes on remote device.
NOTE: If this bit is not set, an AC (or WR+FR) command must be sent before changes will take effect.
All other bits must be set to zero.*/
	struct at_command at;
};

#endif
