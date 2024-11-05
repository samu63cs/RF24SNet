#include "RF24SNet.h"

#ifdef RSNET_CRYPT
  RSNetCrypt rscrypt;
#endif

// Inicializa el objeto RF24SNet didc = %this device id (0 = master)%, wfdt = wait for data time in microseconds (max 65)
RF24SNet::RF24SNet(uint16_t didc, int wfdt, RF24& radio) : radio(radio), did(didc), wait_for_data_ms(wfdt) {
    
}

void RF24SNet::begin(uint8_t pa_level, rf24_datarate_e data_rate, uint8_t channel, byte address[5] )
{
   noInterrupts();
  radio.begin();
  radio.setPALevel(pa_level, true);
  radio.setDataRate(data_rate);
  radio.setChannel(channel); 
  radio.setAutoAck(false);
  radio.openWritingPipe(address);  
  radio.openReadingPipe(0, address);
  radio.startListening();
  
  interrupts();
  
}

bool RF24SNet::handshake(uint16_t idm)
{
    
   DataPayload data;
   data.ids = did;
   data.idr = idm;
   data.command = handshake_c;
   data.msg[0] = '\0';
   sendRawDataPayload(data);
   
   DataPayload dataR = waitForData(idm);

   

   if(dataR.command == handshaker_c)
   {
    return true;
   } else {
    return false;
   }

   
   
}

bool RF24SNet::sendRawDataPayload(DataPayload& data){
   noInterrupts();
  radio.stopListening();
  bool ok = radio.write(&data, sizeof(data));
  radio.startListening();
  interrupts();

  return ok;
}


/// @brief 
/// @param idm //idm specifies which id we are expecting data from 
/// @return Returns a DataPayload (if dataReceived is== false, command = dontgetd_c) (This won't work with TTPayloads) (This function is used to wait data from data Request "Specialy made for master")
DataPayload RF24SNet::waitForData(uint16_t idm){

  DataPayload data;
  bool dataReceived = false;

  unsigned long startMillis = millis();
 noInterrupts();
  while(millis() - startMillis < wait_for_data_ms)
  {
     if (radio.available()) { // Verifica si hay datos disponibles
            radio.read(&data, sizeof(data)); 
            if(data.idr == did)
            if(data.ids == idm)
            {
                dataReceived = true;
                //Serial.println("Got answer at ms: " + String(millis() - startMillis));
                break;
            }
   }
 }
 interrupts();
 

 if(!dataReceived)
 {
    data.command = dontgetd_c;
 }

 return data;
}

/// @brief 
/// @param idm //Request data to %idm% 
/// @return Returns the answer from %idm%
DataPayload RF24SNet::requestData(uint16_t idm)
{

    DataPayload data;
    data.ids = did;
    data.idr = idm;
    data.command = requestd_c;
    data.msg[0] = '\0';

    sendRawDataPayload(data);
    //delayMicroseconds(wait_for_data_mcs);
    DataPayload dataR = waitForData(idm);

   return dataR;
}
/// @brief 
/// @param idm //Send to %idm% 
/// @param msg //String message (max 26 char)
/// @param commandd //Command
/// @return Returns if the send was ok
bool RF24SNet::send(uint16_t idm, String& msg, uint8_t commandd = 0)
{
  DataPayload data;
  data.ids = did;
  data.idr = idm;
  data.command = commandd;
  
    int length = min(msg.length(), sizeof(data.msg) - 1);
    msg.toCharArray(data.msg, length + 1);  // +1 para el terminador nulo

   bool ok = sendRawDataPayload(data);

   return ok;
}
/// @brief Una funcion que maneja los comandos basicos (solicitudes no) y la carga de timestamp si la encriptacion esta activada
/// @return Returns a complete DataPayload if there was a message and if there is not, returns command = dontgetd_c
DataPayload RF24SNet::listen()
{
 DataPayload data;
  bool dataReceived = false;
  
     if (radio.available()) { // Verifica si hay datos disponibles
            radio.read(&data, sizeof(data)); 
            if(data.idr == did)
            {
                dataReceived = true;
                if(data.command == handshake_c)
                {
                    //delayMicroseconds(wait_for_data_mcs);
                    String msg = "";
                    send(data.ids, msg, handshaker_c);
                }
            }
                
            
    }

 if(!dataReceived)
 {
    data.command == dontgetd_c;
 }

 return data;


}


/// @brief 
/// @param data DataPayload
/// @return True if there is data on the payload
bool RF24SNet::isAvailable(DataPayload& data)
{
    if(data.command == dontgetd_c)
    {
        return false;
    } else {
        return true;
    }

    
}
/// @brief 
/// @return Returns true if this device is the master 
bool RF24SNet::imMaster()
{
    return did == master_id;
}