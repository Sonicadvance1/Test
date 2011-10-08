/* 
 * File:   structs.h
 * Author: dynamos
 *
 * Created on October 6, 2011, 1:15 AM
 */
#ifndef STRUCTS_H
#define STRUCTS_H
#define TCP_BUFFER_SIZE 64000

typedef struct item {
	unsigned char data[12]; // item data contained here
	unsigned itemid; // player item id
} ITEM;

typedef struct inventory_item {
	unsigned char in_use; //slot in use flag
	unsigned flags; // 8 = equipped
	ITEM item;
} INVENTORY_ITEM;

typedef struct monster {
	short HP;
	unsigned dead[4];
	unsigned drop;
} MONSTER;


typedef struct playerLevel {
unsigned short Attack;
	unsigned short Mental;
	unsigned short Evade;
	unsigned short HP;
	unsigned short Defense;
	unsigned short Accuracy;
	unsigned short Luck;
	unsigned char TP;
	unsigned XP;
} playerLevel;

typedef struct itemdata {
    unsigned short atkboost;
    int enchantment;
    int type;
    int requiredlvl;
    
} itemdata;

typedef struct pineapple {
	unsigned char lang; // 0x0B
	INVENTORY_ITEM inventory[30];
	unsigned short Attack;
	unsigned short Mental;
	unsigned short Evade;
	unsigned short HP;
	unsigned short Defense;
	unsigned short Accuracy;
	unsigned short Luck;
	unsigned short level;
	unsigned XP;
	unsigned monies;
	unsigned short skinID;
	unsigned char DropID;
	unsigned char _class;
	unsigned short shirt;
    unsigned short pants;
    unsigned short accessories;
	unsigned short skin;
	unsigned short face;
	unsigned short head;
	unsigned short hair;
	//unsigned short hairColorRed; //maybe?
	//unsigned short hairColorBlue; //maybe?
	//unsigned short hairColorGreen; //maybe?
	unsigned char name[24];
	unsigned playTime;
	unsigned char Magicka[20];
	unsigned GuildID;
	unsigned char GuildName[28];
} PINEAPPLE;

typedef struct banana {
	int plySockfd;
	unsigned char rcvbuf [TCP_BUFFER_SIZE];
	unsigned short rcvread;
	unsigned short expect;
	//unsigned char decryptbuf [TCP_BUFFER_SIZE]; // Used when decrypting packets from the client...
	unsigned char sndbuf [TCP_BUFFER_SIZE];
	//unsigned char encryptbuf [TCP_BUFFER_SIZE]; // Used when making packets to send to the client...
	unsigned char packet [TCP_BUFFER_SIZE];
	int snddata, sndwritten;
	PINEAPPLE character;
	int mode;
	int isgm;
	int slotnum;
	unsigned response;		// Last time client responded...
	unsigned lastTick;		// The last second
	unsigned toBytesSec;	// How many bytes per second the server sends to the client
	unsigned fromBytesSec;	// How many bytes per second the server receives from the client
	unsigned packetsSec;	// How many packets per second the server receives from the client
	unsigned char clientID;
	int todc;
	unsigned dc_time;
	unsigned char IP_Address[16]; // Text version
	unsigned char ipaddr[4]; // Binary version
    unsigned connected; //is you here dawg?
    unsigned clientcard; // Pretty much like guildcard is pso (used to distinguish players)
	unsigned savetime;
	unsigned connection_index;
	unsigned drop_area;
	long long drop_coords;
	unsigned drop_item;
	int released;
	unsigned char releaseIP[4];
	unsigned short releasePort;
	int dead;
} BANANA;


#endif
