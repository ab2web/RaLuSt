//////////////////////////////////////////////////
//--------------------RaLuSt--------------------//
//--------------Raumluftsteuerung---------------//
//**********************************************//
//Author: A.Barac-------------------------------//
//**********************************************//
// Version: 0.3---------------------------------//
// Date: 06.04.2015-----------------------------//
//**********************************************//
// File: RaLuSt_init----------------------------//
// Descr.: all initialisation, setup, define    //
//         and as global constants & variables  //
//         are defined here                     //
//////////////////////////////////////////////////

//init#####################################################################################################################
//includes
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EtherCard.h>
#include <Bounce2.h>

//defines
#define ONE_WIRE_BUS A0 //A0 = Pin14 müsste aber auch mit A0 gehen
#define DHTPIN 19
// what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define TCP_FLAGS_FIN_V 1 //as declared in net.h
#define TCP_FLAGS_ACK_V 0x10 //as declared in net.h


///Button defines
#define INPUTMODE INPUT_PULLUP            // INPUT oder INPUT_PULLUP
byte buttonPins[] = {                     // Pin-Nummern der angeschlossenen Buttons
  3, 4, 5, 6, 7};                         // 5=up; 4=left; 3=enter; 7=down; 6=right
#define NUMBUTTONS sizeof(buttonPins)     // Die Anzahl der Tasten durch die Anzahl der Bytes des Arrays buttonPins ermitteln (wird automatisch definiert)

Bounce debouncer[NUMBUTTONS];             // Mehrere Bounce-Instanzen erstellen

boolean buttonState[NUMBUTTONS];          // Speichert den aktuellen HIGH/LOW Status der Pins
byte buttonChange[NUMBUTTONS];            // Speichert die Zustandswechsel an den Pins

enum{
  UNCHANGED, PUSH, RELEASE};

byte buttonPressCount;                    // Speichert wie viele Tasten gleichzeitig gedrückt gehalten werden

byte buttonAction;                        // Gibt an, für welche Einzeltaste eine Aktion ausgelöst wird.

enum {
  NOACTION,                                                    // Index 0 = unbelegt/ keine Aktion
  BUT1PUSH, BUT2PUSH, BUT3PUSH, BUT4PUSH, BUT5PUSH, BUT6PUSH,  // Index 1 bis (NUMBUTTONS) = EinzeltastendruckAktionen
  BUT1HOLD, BUT2HOLD, BUT3HOLD, BUT4HOLD, BUT5HOLD, BUT6HOLD,  // Index (NUMBUTTONS + 1) - (NUMBUTTONS + NUMBUTTONS) = EinzeltastenHalteAktionen
  BUT12HOLD};                                                  // Selbst zu definierende MehrfachtastenHalteAktionen


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

DHT dht(DHTPIN, DHTTYPE);


#define STATIC 0  // set to 1 to disable DHCP (adjust myip/gwip values below)
#define DEBUG 1   // set to 1 to show incoming requests on serial port

#if STATIC
// ethernet interface ip address
static byte myStatip[] = { 192,168,178,200 };
// gateway ip address
static byte gwStatip[] = { 192,168,178,1 };
#endif

//Ethernetboard Einstellungen
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[1000]; // tcp/ip send and receive buffer
BufferFiller bfill;

//char ascii[10];
float temp_in = 0, temp_out = 0, humi_in = 0, humi_out = 0, refresh = 1;

const int relaisPin = 8;      // the number of the pin to toggle the relais
int relaisState = LOW;         // the current state of the output pin
long relaisWaitTime = 0;

//// configuration, as stored in EEPROM
//struct Config {
//    byte band;
//    byte group;
//    byte collect;
//    word refresh;
//    byte valid; // keep this as last byte
//} config;
  
const char page[] PROGMEM =
"HTTP/1.0 503 Service Unavailable\r\n"
"Content-Type: text/html\r\n"
"Retry-After: 600\r\n"
"\r\n"
"<html>"
  "<head><title>"
    "Service Temporarily Unavailable"
  "</title></head>"
  "<body>"
    "<h3>This service is currently unavailable</h3>"
    "<p><em>"
      "The main server is currently off-line.<br />"
      "Please try again later."
    "</em></p>"
  "</body>"
"</html>"
;

const char http_OK[] PROGMEM =
"HTTP/1.0 200 OK\r\n"
"Content-Type: text/html\r\n"
"Pragma: no-cache\r\n\r\n";

const char okHeader[] PROGMEM = 
"HTTP/1.0 200 OK\r\n"
"Content-Type: text/html\r\n"
"Pragma: no-cache\r\n"
;

const char http_Found[] PROGMEM =
"HTTP/1.0 302 Found\r\n"
"Location: /\r\n\r\n";

const char http_Unauthorized[] PROGMEM =
"HTTP/1.0 401 Unauthorized\r\n"
"Content-Type: text/html\r\n\r\n"
"<h1>401 Unauthorized</h1>";


void setup() {
  Serial.begin(9600); 
  //Serial.begin(57600);
  
  pinMode(relaisPin, OUTPUT);

  // set initial relais state
  digitalWrite(relaisPin, relaisState);

  Serial.println(F("DHT11 & DS1820 sowie DHCP Test!"));
  sensors.begin(); 
  dht.begin();
  
#if DEBUG
  //Ethernet Setup_________________________________________
  Serial.print(F("MAC: "));
  for (byte i = 0; i < 6; ++i) 
  {
    Serial.print(mymac[i], HEX);
    if (i < 5)
      Serial.print(':');
  }
  Serial.println();
#endif
  
    if (ether.begin(sizeof Ethernet::buffer, mymac, 10) == 0){
#if DEBUG
    Serial.println( F("Failed to access Ethernet controller"));
#endif
    }

#if STATIC
  ether.staticSetup(myStatip, gwStatip);
#else
  #if DEBUG    
      Serial.println(F("Setting up DHCP"));
  #endif
      if (!ether.dhcpSetup()){
  #if DEBUG
      Serial.println(F("DHCP failed"));
  #endif
      }
#endif



  ether.printIp("My IP: ", ether.myip);
  ether.printIp("Netmask: ", ether.netmask);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);
  //END Ethernet____________________________________________
  
  
  for(int i = 0; i < NUMBUTTONS; i++)
  {
    pinMode(buttonPins[i], INPUTMODE);
    debouncer[i].attach(buttonPins[i]);
    debouncer[i].interval(10);
  }
}

//init end#####################################################################################################################






//Freien Speicher ermitteln zur anzeige auf der Info Webpage
static int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}









//////////////////////////////////////////////////
//--------------------RaLuSt--------------------//
//--------------Raumluftsteuerung---------------//
//**********************************************//
//Author: A.Barac-------------------------------//
//**********************************************//
// Version: 2.3---------------------------------//
// Date: 06.04.2015-----------------------------//
//**********************************************//
// File: RaLuSt_main----------------------------//
// Descr.: the main routine                     //
//////////////////////////////////////////////////
//main#####################################################################################################################

void loop() {
  
  readDHT();
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(temp_in) || isnan(temp_out) || isnan(humi_in) || isnan(humi_out)) {
    Serial.println(F("Fehler beim lesen der Werte des DHT"));
  } else {
    
    #if DEBUG
    Serial.print(F("Feuchtigkeit: ")); 
    Serial.print(humi_in);
    Serial.print(F(" %\t"));
    Serial.print(F("Temperatur: ")); 
    Serial.println(temp_in);
    Serial.println(round(temp_in));
    Serial.println(F(" *C"));
    #endif

////ASCII Konvertierung, falls man Sie doch mal brauch.
////    dtostrf(t,7,0,ascii);
////    for(int i=0;i<sizeof(ascii);i++)
////  {
////    o+=ascii[i];
////  }
////    Serial.print("ASCII-Wert ist: ");
////    Serial.println(o);
  }
  
  readDallas();

  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  
        if (pos) {
        bfill = ether.tcpOffset();
        char* data = (char *) Ethernet::buffer + pos;

#if DEBUG
        Serial.println(data);
#endif
        // receive buf hasn't been clobbered by reply yet
        if (strncmp("GET / ", data, 6) == 0)
            homePage();
        else if (strncmp("GET /overview", data, 6) == 0)
            //configPage(data, bfill);
              bfill.emit_p(PSTR(
                "HTTP/1.0 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "<title>Arduino Ralust Server</title>"
                "<h1>Overview</h1>"
                "<h2>Temperature</h2>"                    
                "<h3>Temp. in: $D C</h3>"
                "<h3>Temp. out: $D C</h3>"
                "<h2>Humidity</h2>" 
                "<h3>Humi. in: $D%</h3>"
                "<h3>Humi. out: $D%</h3>"),
                temp_in, temp_out, humi_in, humi_out); 
        else if (strncmp("GET /s", data, 6) == 0)
            //sendPage(data, bfill);
              bfill.emit_p(PSTR(
                "HTTP/1.0 200 Unauthorized\r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "<h1>Temperatur</h1>"));
        else if (strncmp("GET /config", data, 6) == 0){
            configPage(data, bfill);
             ether.httpServerReply(bfill.position()); // send web page data
             }
        else if (strncmp("GET /info", data, 6) == 0){
            infoPage(data, bfill);
            ether.httpServerReply(bfill.position()); // send web page data
        }
        else if (strncmp("GET /relais", data, 6) == 0){
            relais(data, bfill);
            ether.httpServerReply(bfill.position()); // send web page data
        }
        else{
            bfill.emit_p(PSTR(
                "HTTP/1.0 401 Unauthorized\r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "<h1>401 Unauthorized</h1>"));  
            ether.httpServerReply(bfill.position()); // send web page data
        }
    }


  ///Buttons abfragen
    eingabe();
    verarbeitung();
    ausgabe();
    setRelais();

    
  delay(refresh);


}
