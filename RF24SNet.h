// lib/RF24SNet
#include <Arduino.h>
#include <RF24.h>  // Incluye la librería RF24 si la estás usando
#include <nRF24L01.h>



////IF YOU WANT TO CRYPT - SI quieres encriptar





#ifndef RF24SNET_H
#define RF24SNET_H

    struct DataPayload {
    uint16_t ids = 0; //id sender
    uint16_t idr = 0; //id receiver            
    uint8_t command = 0;      
    char msg[27];       
    };
    struct TTPayload
    {
    unsigned long timestamp = 0;
    unsigned int randomizer = 0;  
    };
    union Payload {
    DataPayload data;
    TTPayload tt;
};
    

class RF24SNet {
public:
    RF24SNet(uint16_t didc, int wfdt, RF24& radio);

    void begin(uint8_t pa_level, rf24_datarate_e data_rate, uint8_t channel, byte address[5] );
    bool handshake(uint16_t idm);
    bool sendRawDataPayload(DataPayload& data);
    DataPayload waitForData(uint16_t idm);
    DataPayload requestData(uint16_t idm);
    bool send(uint16_t idm, String& msg, uint8_t commandd = 0);
    DataPayload listen();
    bool isAvailable(DataPayload& data);
    bool imMaster();




    
    uint8_t requestd_c = 0;
    uint8_t requestdr_c = 1;
    uint8_t handshake_c = 2;
    uint8_t handshaker_c = 3;
    
    uint8_t dontgetd_c = 10;
    

    

private:
uint16_t did = 0;
int wait_for_data_ms;
int ack_retries;
int ack_delay_ms;
RF24& radio;
   

};


/*
#ifdef RSNET_CRYPT
#include "RSNetCrypt.h"
#endif*/


#endif