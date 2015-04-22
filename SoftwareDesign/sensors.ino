//////////////////////////////////////////////////
//--------------------RaLuSt--------------------//
//--------------Raumluftsteuerung---------------//
//**********************************************//
//Author: A.Barac-------------------------------//
//**********************************************//
// Version: 1.0---------------------------------//
// Date: 06.04.2015-----------------------------//
//**********************************************//
// File: RaLuSt_Sensors-------------------------//
// Descr.: all Methods for reading sensors      //
//////////////////////////////////////////////////

//DHT PINOut: nicht A7 und nicht A6 beim Nano verwenden... 
// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor


void readDHT() {
  //Reading temperature or humidity takes about 250 milliseconds!
  //Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humi_in = dht.readHumidity(); //float h = dht.readHumidity();
  temp_in = dht.readTemperature(); //float t = dht.readTemperature();
  Serial.println(humi_in);
  Serial.println(temp_in); 
  delay(2000);
}


float readDallas(){
  
 // #if DEBUG
  Serial.print(F("Frage Temperatur ab..."));
 // #endif
  
  sensors.requestTemperatures(); // Send the command to get temperatures
  
 // #if DEBUG
  Serial.print(F("Temperatur device 1 (index 0) ist: "));
  Serial.println(sensors.getTempCByIndex(0));
 // #endif

  return sensors.getTempCByIndex(0);
}



//Todo: DHT22 fertig implementieren...
//_________________
//
//void Dht22()
//{ 
//  DHT22_ERROR_t fehlerCodeDHT22;
//  Serial.print("Hole Daten DHT22 ...: ");
//  fehlerCodeDHT22 = DHT22_1.readData(); // hole Status
//  switch(fehlerCodeDHT22) // werte Status aus
//  {
//    case DHT_ERROR_NONE:; // wenn kein Fehler
//      temperaturDHT22 = DHT22_1.getTemperatureC();
//      Serial.print(temperaturDHT22);
//      Serial.print(" C ");
//      feuchtigkeit = DHT22_1.getHumidity();
//      Serial.print(feuchtigkeit);
//      Serial.println(" %");
//      Taupunkt();
//      break;
//    case DHT_ERROR_CHECKSUM:
//      Serial.print("Fehler Pruefziffer ");
//      break;
//    case DHT_BUS_HUNG:
//      Serial.println("BUS haengt ");
//      break;
//    case DHT_ERROR_NOT_PRESENT:
//      Serial.println("nicht vorhanden ");
//      break;
//    case DHT_ERROR_ACK_TOO_LONG:
//      Serial.println("Fehler ACK Timeout ");
//      break;
//    case DHT_ERROR_SYNC_TIMEOUT:
//      Serial.println("Fehler Sync Timeout ");
//      break;
//    case DHT_ERROR_DATA_TIMEOUT:
//      Serial.println("Fehler Daten Timeout ");
//      break;
//    case DHT_ERROR_TOOQUICK:
//      Serial.println("Abfrage zu schnell ");
//      break;
//  }  
//}


void Taupunkt() // berechne Taupunkt (Naeherungsformel)
{
   float a = 17.271;
   float b = 237.7;
   //float temperatur = (a * temperaturDHT22) / (b + temperaturDHT22) + log(feuchtigkeit/100);
   //float taupunkt = (b * temperatur) / (a - temperatur);
   Serial.print(F("Taupunkt: "));
   //Serial.println(taupunkt); 
}
