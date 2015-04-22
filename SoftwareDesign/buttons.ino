/* 

Anleitung:

1. Im Array "buttonPins" die Pins eintragen, an denen jeweils ein Taster hängen soll. (Index 0 = Taste 1)
2. Entprellzeit festlegen.
3. Die zweite ENUM-Liste an die Anzahl der Tasten anpassen.
4. Halteaktionen für das gedrückt halten mehrerer Tasten selbst definieren.

*/


void eingabe()  // Tastenpins auslesen und deren Status mit buttonState und buttonChange an die Verarbeitung übergeben (Reicht bereits für einfachste Anwendungen)
{
  memset(buttonChange,0,sizeof(buttonChange));         // Alle alten Zustandswechsel verwerfen (buttonChange wird deshalb komplett mit Nullen überschrieben)
  for(int i = 0; i < NUMBUTTONS; i++)
  {
    debouncer[i].update();   // Status prüfen
    if(debouncer[i].fell())  // Wenn die Taste gedrückt wird
    {
      buttonChange[i] = PUSH;
      buttonState[i] = 1;
    }
    if(debouncer[i].rose())  // Wenn die Taste losgelassen wird
    {
      buttonChange[i] = RELEASE;
      buttonState[i] = 0;
    }
  }
}



void verarbeitung()
{
  static boolean buttonActionCheck;     // Hält fest, ob bereits eine EinzelTastenaktion stattgefunden hat (0= noch keine Aktion, 1= Aktion fand bereits statt)
  static unsigned long buttonPushTime;  // Gibt den Zeitpunkt an, wann zuletzt eine Taste gedrückt wurde
  static boolean actionsBlocked;        // Gibt an, ob weitere Aktionen unterbunden werden sollen

  if(!buttonPressCount)       // Sobald keine Taste mehr gedrückt gehalten wird
  {                           // Dieser if-Block (inkl. memset) bildet den Abschluss des eigentlichen Codes
    buttonActionCheck = 0;    // Reset
    buttonPressCount = 0;     // Reset
    actionsBlocked = 0;       // Reset
  }
  buttonAction = NOACTION;    // AktionsByte zurücksetzen damit die letzte Aktion nicht mehrmals ausgelöst wird.

  for(int i = 0; i < NUMBUTTONS; i++)
  {
    if(buttonChange[i] == 1)  // Taste wurde gedrückt
    {
      if(buttonPressCount < NUMBUTTONS) buttonPressCount++;  // Wenn die Taste gedrückt wird, die Anzahl der gleichzeitig gedrückt gehaltenen Tasten inkrementieren.
      buttonPushTime = millis();                             // Den Zeitpunkt festhalten, wann eine Taste gedrückt wurde  
      if(!buttonActionCheck)                                 // Wenn noch keine Aktion für das drücken einer Taste ausgelöst wurde
      {
        buttonActionCheck++;   // Aktionszähler inkrementieren
        buttonAction = i + 1;  // Aktion (im weiteren Verlauf des Sketchs) auslösen (0 = keine Aktion)
      }
    }

    if(buttonChange[i] == 2)  // Taste wurde losgelassen
    {
      if(buttonPressCount) buttonPressCount--;  // Wenn die Taste losgelassen wird, die Anzahl der gleichzeitig gedrückt gehaltenen Tasten dekrementieren.
      actionsBlocked = 1;                       // Sorgt dafür, dass keine Tastenaktionen mehr ausgeführt werden können.
    }

    if(!actionsBlocked)
    {
      if(millis() - buttonPushTime >= 1200)
      {
        if(buttonState[i] && buttonPressCount == 1)  // Halteaktionen der einzelnen Tasten
        {
          buttonAction = i + NUMBUTTONS + 1;   // Aktion (im weiteren Verlauf des Sketchs über ENUM) auslösen (s.o. ENUM-Liste)
          actionsBlocked = 1;                  // Sorgt dafür, dass keine Tastenaktionen mehr ausgeführt werden können.
        }
        if(buttonState[0] && buttonState[1] && buttonPressCount == 2)  // Definierte Halteaktion für mehrere Tasten
        {
          buttonAction = BUT12HOLD;
          actionsBlocked = 1;         // Sorgt dafür, dass keine Tastenaktionen mehr ausgeführt werden können.
        }
      }
    }
  }
}



void ausgabe()
{
  if(buttonAction == BUT1PUSH){
    Serial.println(F("BUT1PUSH"));

      // only toggle the LED if the new button state is HIGH
//      if (relaisState == HIGH) {
//        digitalWrite(relaisPin, LOW); }
//
//      if (relaisState == LOW) {
//        digitalWrite(relaisPin, HIGH);}

        digitalWrite(relaisPin, HIGH);
        delay(200);
        digitalWrite(relaisPin, LOW);
       
  }
  if(buttonAction == BUT2PUSH) Serial.println(F("BUT2PUSH"));
  if(buttonAction == BUT3PUSH) Serial.println(F("BUT3PUSH"));
  if(buttonAction == BUT4PUSH) Serial.println(F("BUT4PUSH"));
  if(buttonAction == BUT5PUSH) Serial.println(F("BUT5PUSH"));
  if(buttonAction == BUT6PUSH) Serial.println(F("BUT6PUSH"));

  if(buttonAction == BUT1HOLD) Serial.println(F("BUT1HOLD"));
  if(buttonAction == BUT2HOLD) Serial.println(F("BUT2HOLD"));
  if(buttonAction == BUT3HOLD) Serial.println(F("BUT3HOLD"));
  if(buttonAction == BUT4HOLD) Serial.println(F("BUT4HOLD"));
  if(buttonAction == BUT5HOLD) Serial.println(F("BUT5HOLD"));
  if(buttonAction == BUT6HOLD) Serial.println(F("BUT6HOLD"));

  if(buttonAction == BUT12HOLD) Serial.println(F("BUT12HOLD"));
  
  static unsigned long lasttime;
  if(millis() - lasttime >= 1000)
  {
#if DEBUG
    Serial.print(F("\t\t\tAnzahl gedrueckt gehaltener Tasten: "));
    Serial.println(buttonPressCount);
#endif
    lasttime = millis();
  }
}

void setAllOutPut()
{
  //if (relaisState)
   //digitalWrite(relaisPin, HIGH)
    //digitalWrite(relaisPin, relaisState)
}


void setRelais()
{
  if(relaisWaitTime!=0){         //es wurde eine Rücksetzzeit angegeben, deshalb ...
#if DEBUG 
  Serial.print(F("Rücksetzen in: "));
  Serial.println(relaisWaitTime);
#endif
    if(millis()<=relaisWaitTime){//... prüfen ob noch innerhalb dieser und wenn ja dann...
      digitalWrite(relaisPin, relaisState); //... setze neuen Zustand
#if DEBUG
      Serial.print(F("noch nicht, relaisSate weiterhin: "));
      Serial.println(relaisState);
#endif      
    }
    else{                       //wenn nicht mehr innerhalb der relaisWaitTime, dann
#if DEBUG
      Serial.print(F("relaisSate wir zurück gesetzt von: "));
      Serial.println(relaisState);
#endif    
      if(relaisState==0) relaisState=1;  //falls relaisState 0 war setze es zurück auf 1
      if(relaisState==1) relaisState=0;  //falls relaisState 0 war setze es zurück auf 1
      relaisWaitTime = 0;
#if DEBUG
      Serial.print(F("Auf: "));
      Serial.println(relaisState);
#endif
    }
  }
  else{                          //Es wurde keine Rücksetzzeit angegeben, deshalb einfach nur den neuen Zustand setzen
    digitalWrite(relaisPin, relaisState);
  }
  
  
}

