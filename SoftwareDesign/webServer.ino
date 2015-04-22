//////////////////////////////////////////////////
//--------------------RaLuSt--------------------//
//--------------Raumluftsteuerung---------------//
//**********************************************//
//Author: A.Barac-------------------------------//
//**********************************************//
// Version: 2.3---------------------------------//
// Date: 06.04.2015-----------------------------//
//**********************************************//
// File: RaLuSt_WebServer-----------------------//
// Descr.: holy webpage handling----------------//
//////////////////////////////////////////////////


//html Inhalte____________________________________
const char configA[] PROGMEM =
           "<h3>Server node configuration</h3>"
           "<form>"
           "<p>";
const char configB[] PROGMEM ="IP-Adress: <input type=text name=i0 value='$D' size=1>."
           "<input type=text name=i1 value='$D' size=1>."
           "<input type=text name=i2 value='$D' size=1>."
           "<input type=text name=i3 value='$D' size=1> (192.168.178.xx)<br>";
const char configC[] PROGMEM =
           "GW-IP: <input type=text name=g0 value='$D' size=1>."
           "<input type=text name=g1 value='$D' size=1>."
           "<input type=text name=g2 value='$D' size=1>."
           "<input type=text name=g3 value='$D' size=1> (192.168.178.1)<br>";
//const char configD[] PROGMEM =
//           "Netmask: (255.255.255.<input type=text name=n3 value='$D' size=1>) <br>"
//           "DNSIP: <input type=text name=d0 value='$D' size=1>." 
//           "<input type=text name=d1 value='$D' size=1>."
//           "<input type=text name=d2 value='$D' size=1>."
//           "<input type=text name=d3 value='$D' size=1> (192.168.178.1)<br>"
//           ), ether.netmask[3],
//        ether.dnsip[0], ether.dnsip[1], ether.dnsip[2], ether.dnsip[3]);
//const char configE[] PROGMEM =
//           PSTR("Debug: <input type=checkbox name=c value='1' $S> "
//           "Print serial output<br><br>"
//           "Refresh rate <input type=text name=refr value='$D' size=4> (1..3600 seconds)"
//          "</p>"
//          "<input type=submit value=Set>"
//        "</form>"),
//        DEBUG ? "CHECKED" : "",
//        refresh);
//______________________________________________



#define MAX_STRING 60
char stringBuffer[MAX_STRING];

char* getString(const char* str){
  strcpy_P(stringBuffer, (char*)str);
  return stringBuffer;
}



static word homePageOld() {
  long t = millis() / 1000;
  //word h = t / 3600;
  //long h = dht.readHumidity();
  //float l = dht.readTemperature(); 
  //long l = (long) dht.readTemperature(); 
  
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "<meta http-equiv='refresh' content='1'/>"
    "<title>RaLuST server</title>" 
    "<h3>Feuchtigkeit: $D%    Temperatur: $D C</h3>"),
      humi_in, temp_in); 
     // h,o);
  return bfill.position();
}

void homePage(){
  //$F represents p string, $D represents word or byte, $S represents string. 
  //There are 5 leds, so 5 hyperlinks.
  bfill.emit_p(PSTR("$F"
    "<meta http-equiv='refresh' content='5'/>"
    "<title>Ethercard LED</title>"),
//    "Turn LED$D <a href=\"?led=$S\">$S</a>" 
//    "Turn LED$D <a href=\"?led=$S\">$S</a>" 
//    "Turn LED$D <a href=\"?led=$S\">$S</a>" 
//    "Turn LED$D <a href=\"?led=$S\">$S</a>" 
//    "Turn LED$D <a href=\"?led=$S\">$S</a>"
 //   ),
  http_OK);
//  (word)1,LedURL[0],(LedURL[0][0]=='1')?"ON":"OFF",
//  (word)2,LedURL[1],(LedURL[1][1]=='1')?"ON":"OFF",
//  (word)3,LedURL[2],(LedURL[2][2]=='1')?"ON":"OFF",
//  (word)4,LedURL[3],(LedURL[3][3]=='1')?"ON":"OFF",
//  (word)5,LedURL[4],(LedURL[4][4]=='1')?"ON":"OFF"); 
}


static int getIntArg(const char* data, const char* key, int value =-1) {
    char temp[10];
    if (ether.findKeyVal(data + 12, temp, sizeof temp, key) > 0)
        value = atoi(temp);
    return value;
}


static void configPage(const char* data, BufferFiller& buf) {
    // pick up submitted data, if present
    Serial.println(data);
    if (data[11] == '?') {
        ether.myip[0] = getIntArg(data, "i0");
        ether.myip[1] = getIntArg(data, "i1");
        ether.myip[2] = getIntArg(data, "i2");
        ether.myip[3] = getIntArg(data, "i3");
        ether.gwip[0] = getIntArg(data, "g0");
        ether.gwip[1] = getIntArg(data, "g1");
        ether.gwip[2] = getIntArg(data, "g2");
        ether.gwip[3] = getIntArg(data, "g3");
        ether.dnsip[0] = getIntArg(data, "d0");
        ether.dnsip[1] = getIntArg(data, "d1");
        ether.dnsip[2] = getIntArg(data, "d2");
        ether.dnsip[3] = getIntArg(data, "d3");
        ether.netmask[3] = getIntArg(data, "n3");
        //byte g = getIntArg(data, "g");
        //byte c = getIntArg(data, "c", 0);
        refresh = getIntArg(data, "refr");
        //Serial.println(refresh);
        ether.printIp("My IP: ", ether.myip);
        ether.printIp("Netmask: ", ether.netmask);
        ether.printIp("GW IP: ", ether.gwip);
        ether.printIp("DNS IP: ", ether.dnsip);
//        if (1 <= g && g <= 250 && 1 <= r && r <= 3600) {
//            // store values as new settings
//            config.band = b;
//            config.group = g;
//            config.collect = c;
//            config.refresh = r;
//            saveConfig();
//            // re-init RF12 driver
//            loadConfig();
//            // clear history
//            memset(history_len, 0, sizeof history_len);
            // redirect to the home page
            buf.emit_p(PSTR(
                "HTTP/1.0 302 found\r\n"
                "Location: /\r\n"
                "\r\n"));
            return;
        //}
    }
    
//    // ether.httpServerReplyAck(); // send ack to the request
//     //Serial.println(getString(configA));
//  //   buf.emit_p(PSTR("$F"), okHeader);
//     ether.httpServerReply_with_flags(sizeof okHeader -1,TCP_FLAGS_ACK_V);
//     buf.emit_p(PSTR("$F"), configA);
//     ether.httpServerReply_with_flags(sizeof configA -1,TCP_FLAGS_ACK_V|TCP_FLAGS_FIN_V);
     
     
//     memcpy_P(ether.tcpOffset(), package, sizeof package); // send first packet and not send the terminate flag
//     ether.httpServerReply_with_flags(sizeof package - 1,TCP_FLAGS_ACK_V);
//     
//     package[] = PSTR(configB), ether.myip[0], ether.myip[1], ether.myip[2], ether.myip[3]);
//     memcpy_P(ether.tcpOffset(), package, sizeof package); // send second packet and not send the terminate flag
//     ether.httpServerReply_with_flags(sizeof package - 1,TCP_FLAGS_ACK_V);
//     
//     package[] = PSTR(configC), ether.gwip[0], ether.gwip[1], ether.gwip[2], ether.gwip[3]);
//     memcpy_P(ether.tcpOffset(), package, sizeof package); // send thirdt packet and not send the terminate flag
//     ether.httpServerReply_with_flags(sizeof package - 1,TCP_FLAGS_ACK_V|TCP_FLAGS_FIN_V);
//     
     
//     memcpy_P(ether.tcpOffset(), configD, sizeof configD); // send fourth packet and not send the terminate flag
//     ether.httpServerReply_with_flags(sizeof configD - 1,TCP_FLAGS_ACK_V);
//     
//     
//     memcpy_P(ether.tcpOffset(), configE, sizeof configE); // send fiveth packet and send the terminate flag
//     ether.httpServerReply_with_flags(sizeof configE - 1,TCP_FLAGS_ACK_V|TCP_FLAGS_FIN_V); }
//    // else show a configuration form
    
    
      buf.emit_p(PSTR("$F\r\n"
          "<h3>Server node configuration</h3>"
          "<form>"
            "<p>"
             "IP-Adress: <input type=text name=i0 value='$D' size=1>."
             "<input type=text name=i1 value='$D' size=1>."
             "<input type=text name=i2 value='$D' size=1>."
             "<input type=text name=i3 value='$D' size=1> (192.168.178.xx)<br>"
             ), okHeader, 
          ether.myip[0], ether.myip[1], ether.myip[2], ether.myip[3]);
        
  ////      ether.httpServerReply_with_flags(ether.bufferSize,TCP_FLAGS_ACK_V);

    buf.emit_p(PSTR(
           "GW-IP: <input type=text name=g0 value='$D' size=1>."
           "<input type=text name=g1 value='$D' size=1>."
           "<input type=text name=g2 value='$D' size=1>."
           "<input type=text name=g3 value='$D' size=1> (192.168.178.1)<br>"),
           ether.gwip[0], ether.gwip[1], ether.gwip[2], ether.gwip[3]); 
    
    buf.emit_p(PSTR(
           "Netmask: (255.255.255.<input type=text name=n3 value='$D' size=1>) <br>"
           "DNSIP: <input type=text name=d0 value='$D' size=1>." 
           "<input type=text name=d1 value='$D' size=1>."
           "<input type=text name=d2 value='$D' size=1>."
           "<input type=text name=d3 value='$D' size=1> (192.168.178.1)<br>"
           ), ether.netmask[3],
        ether.dnsip[0], ether.dnsip[1], ether.dnsip[2], ether.dnsip[3]);

    buf.emit_p(PSTR(
           "Debug: <input type=checkbox name=c value='1' $S> "
           "Print serial output<br><br>"
           "Refresh rate <input type=text name=refr value='$D' size=4> (1..3600 seconds)"
          "</p>"
          "<input type=submit value=Set>"
        "</form>"),
        DEBUG ? "CHECKED" : "",
        refresh);
}



///relaisPage activates the relais for the given period
static void relais(const char* data, BufferFiller& buf) {
#if DEBUG
  Serial.println(data);
#endif
    if (data[11] == '?') {
        relaisState = getIntArg(data, "s");
        int period = getIntArg(data, "p");
//    if (relaisState == 1){
    long actSecs = millis() / 1000;
    long duration = actSecs + period;
    
    relaisWaitTime=duration*1000;
    
    //Blockt den Prozessablauf, deshalb ausgelagert in Main und über Flag gesetzt...
//    while(actSecs<duration){
//      digitalWrite(relaisPin, HIGH);
//      actSecs = millis()/1000;
//    }
//    digitalWrite(relaisPin,LOW);



      //After all releais handling is done, redirect to the info Page immediately. 
        buf.emit_p(PSTR("$F\r\n"
        "<meta http-equiv='refresh' content='0;url=info' />"
        "<title>RaLuSt relaisPage</title>"), okHeader);
 //   }        
    }
}
     

///infoPage shows some resource Information of arduino
static void infoPage(const char* data, BufferFiller& buf) {
    long t = millis() / 1000;
    word h = t / 3600;
    byte m = (t / 60) % 60;
    byte s = t % 60;
    buf.emit_p(PSTR("$F\r\n"
        "<meta http-equiv='refresh' content='1'/>"
        "<title>RaLuSt InfoPage</title>" 
        "Menu:"
        " <a href='/'>home</a> |"
        " <a href='config'>configuration</a> |"
        " <a href='info'>this site</a> |"
        " <a href='relais?s=1&p=1'>RelaisOn</a> |"
        " <a href='overview'>sensor overview</a> </br>"
        "<h3>System Information</h3>"
        "<pre>"), okHeader);    
    buf.emit_p(PSTR(
        "</pre>"
        "Uptime is $D$D:$D$D:$D$D </br>"), h/10, h%10, m/10, m%10, s/10, s%10);
    buf.emit_p(PSTR("RAM: ($D bytes free) </br>"), freeRam());
 }

