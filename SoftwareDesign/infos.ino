//#include <EtherCard.h>
//
//#define STATIC 0  // set to 1 to disable DHCP (adjust myip/gwip values below)
//
//#if STATIC
//// ethernet interface ip address
//static byte myip[] = { 
//  192,168,1,200 };
//// gateway ip address
//static byte gwip[] = { 
//  192,168,1,1 };
//#endif
//
//// ethernet mac address - must be unique on your network
//static byte mymac[] = { 
//  0x74,0x69,0x69,0x2D,0x30,0x31 };
//
//byte Ethernet::buffer[500]; // tcp/ip send and receive buffer
//BufferFiller bfill;
//
//
//#define numberOfLeds 5
//int LED[numberOfLeds] = { 2,3,4,5,6}; //This would use digital2 to digital6, but you could choose any 
//char LedURL[numberOfLeds][numberOfLeds+1] ={ "10000", "01000", "00100", "00010", "00001"}; //All leds begin off, so the LEDs in the array are off except for the one which a given hyperlinks would turn on.
//
//void setup(){
//  for(byte i = 0; i < numberOfLeds; i++){
//    pinMode(LED[i],OUTPUT); //LEDs are outputs
//    digitalWrite(LED[i],LOW); //Off to begin with
//  }
//
//  Serial.begin(57600);
//  Serial.println("\n[backSoon]");
//
//  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
//    Serial.println( "Failed to access Ethernet controller");
//#if STATIC
//  ether.staticSetup(myip, gwip);
//#else
//  if (!ether.dhcpSetup())
//    Serial.println("DHCP failed");
//#endif
//
//  ether.printIp("IP:  ", ether.myip);
//  ether.printIp("GW:  ", ether.gwip);  
//  ether.printIp("DNS: ", ether.dnsip);  
//}
//const char http_OK[] PROGMEM =
//"HTTP/1.0 200 OK\r\n"
//"Content-Type: text/html\r\n"
//"Pragma: no-cache\r\n\r\n";
//
//const char http_Found[] PROGMEM =
//"HTTP/1.0 302 Found\r\n"
//"Location: /\r\n\r\n";
//
//const char http_Unauthorized[] PROGMEM =
//"HTTP/1.0 401 Unauthorized\r\n"
//"Content-Type: text/html\r\n\r\n"
//"<h1>401 Unauthorized</h1>";
//
//void homePage(){
//  //$F represents p string, $D represents word or byte, $S represents string. 
//  //There are 5 leds, so 5 hyperlinks.
//  bfill.emit_p(PSTR("$F"
//    "<meta http-equiv='refresh' content='5'/>"
//    "<title>Ethercard LED</title>" 
//    "Turn LED$D <a href=\"?led=$S\">$S</a>" 
//    "Turn LED$D <a href=\"?led=$S\">$S</a>" 
//    "Turn LED$D <a href=\"?led=$S\">$S</a>" 
//    "Turn LED$D <a href=\"?led=$S\">$S</a>" 
//    "Turn LED$D <a href=\"?led=$S\">$S</a>"),
//  http_OK,
//  (word)1,LedURL[0],(LedURL[0][0]=='1')?"ON":"OFF",
//  (word)2,LedURL[1],(LedURL[1][1]=='1')?"ON":"OFF",
//  (word)3,LedURL[2],(LedURL[2][2]=='1')?"ON":"OFF",
//  (word)4,LedURL[3],(LedURL[3][3]=='1')?"ON":"OFF",
//  (word)5,LedURL[4],(LedURL[4][4]=='1')?"ON":"OFF"); 
//  //I forgot in these that we are checking for '1' and '0', not 1 and 0. I also got the indecies wrong 
//  //(I was using MATLAB prior to writing this and they use 1 indexing not 0 indexing so I got confused :S
//}
//
//void loop(){
//  // DHCP expiration is a bit brutal, because all other ethernet activity and
//  // incoming packets will be ignored until a new lease has been acquired
//  if (!STATIC &&  ether.dhcpExpired()) {
//    Serial.println("Acquiring DHCP lease again");
//    ether.dhcpSetup();
//  }
//  // wait for an incoming TCP packet, but ignore its contents
//  word len = ether.packetReceive();
//  word pos = ether.packetLoop(len); 
//  if (pos) {
//    // write to LED digital output
//
//    delay(1);   // necessary for my system
//    bfill = ether.tcpOffset();
//    char *data = (char *) Ethernet::buffer + pos;
//    if (strncmp("GET /", data, 5) != 0) { //I also accidentally copied this bit in twice (though that shouldn't be an issue)
//      // Unsupported HTTP request
//      // 304 or 501 response would be more appropriate
//      bfill.emit_p(http_Unauthorized);
//    } else {
//        data += 5;
//        if (data[0] == ' ') { //Check if the home page, i.e. no URL
//          homePage();
//        } else if (!strncmp("?led=",data,5)){ //Check if a url which changes the leds has been recieved
//          data += strcspn (data,"01"); //Move to the start of the URL
//          char tempURL[numberOfLeds+1] = {0};
//          strncpy(tempURL, data, numberOfLeds); //Extract the recieved URL to the temporary string 
//          Serial.print("temp = ");
//          Serial.println(tempURL); //Just some quick logging
//          for(byte i = 0; i < numberOfLeds ; i++){ //Check through each of the LEDs URLs
//            if(!strcmp(tempURL,LedURL[i])){
//              //The recieved URL matches the one required to turn this LED on, so we will turn it on.
//              digitalWrite(LED[i],(tempURL[i] == '1')?HIGH:LOW); //Set the led state to match the URL.
//              //Now we need to toggle the state of this LED in all of the URLs, so that all the hyperlinks can be corrected to match this state.
//              for(byte j = 0; j < numberOfLeds ; j++){
//                if(j==i){
//                  LedURL[j][i] = (tempURL[i] == '1')?'0':'1'; //Notice how we toggle the 'i'th led in each url.
//                  Serial.print("led = ");
//                  Serial.println(LedURL[j]); 
//                  continue;
//                }
//                LedURL[j][i] = tempURL[i]; //Notice how we toggle the 'i'th led in each url.
//                Serial.print("led = ");
//                Serial.println(LedURL[j]);     
//              }
//              //The URL was found, so we don't need to check any others.
//              break; //Exit the for loop.
//            }          
//          }
//          bfill.emit_p(http_Found);
//        } else { //Otherwise, page isn't found
//          // Page not found
//          bfill.emit_p(http_Unauthorized);
//        }
//      }
//
//      ether.httpServerReply(bfill.position());    // send http response
//    
//  }
//}
