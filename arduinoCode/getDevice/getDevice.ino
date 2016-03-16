//***********************CC3000 Libraries
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>


// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11

//Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER); // you can change this clock speed but DI
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIV2);

#define WLAN_SSID       "YOUR NETWORK"        // cannot be longer than 32 characters!
#define WLAN_PASS       "YOUR PASSWORD"
#define WLAN_SECURITY   WLAN_SEC_WPA2      // Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2

#define IDLE_TIMEOUT_MS  3000

//*************************lightPixel Code
   

long previousMillis = 0; 
long interval = 2000;    // interval at which to reset (milliseconds)
unsigned long currentMillis = 0;

int ledPin = 6;

uint32_t ip;
//Adafruit_CC3000_Client client;

void setup() {

  Serial.begin(9600);
  delay(3000);

  //Initialize CC3000
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin()) {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  // Connect to WiFi network
  Serial.print(F("Connecting to WiFi network ..."));
  cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY);
  Serial.println(F("done!"));

  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    //delay(100);
  }
  Serial.println(F("done!"));

}

void loop() {
  currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    Read();
    previousMillis = currentMillis; // save the last time you reset
  }
}


void Read() {
  
  // GET IP
  uint32_t ip = 0;
  Serial.print(F("YOUR HOST -> "));
  while  (ip  ==  0)  {
    if  (!  cc3000.getHostByName("YOUR HOST", &ip))  {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }  
  cc3000.printIPdotsRev(ip);
  Serial.println(F(""));

  // Check connection to WiFi
  Serial.print(F("Checking WiFi connection ..."));
  if(!cc3000.checkConnected()){
    while(1){
    }
  }
  Serial.println(F("done."));

  // GET REQUEST
  Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 80);
  if (www.connected()) {
    www.fastrprint(F("GET "));
    www.fastrprint(".../api/get/soundforlight");
    www.fastrprint(F(" HTTP/1.1\r\n"));
    www.fastrprint(F("Host: ")); www.fastrprint("YOUR HOST"); www.fastrprint(F("\r\n"));
    www.fastrprint(F("\r\n"));
    www.println();
  }
  else {
  Serial.println(F("Connection failed"));    
  return;
  }

  // READ ANSWER FROM GET REQUEST
  Serial.println(F("Reading answer..."));
  /* Read data until either the connection is closed, or the idle timeout is reached. */ 
  unsigned long lastRead = millis();

  while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    String test = "";
    while (www.available()) {
      char c = www.read();
      test += c;
      Serial.print(c);
      lastRead = millis();
    }
    if (test.indexOf('$') > -1){
      int idx = test.indexOf('$');
      //Serial.print("Current Idx: ");
      //Serial.println(idx);
      int start = idx+1;
      int stop_val = idx+3;
      String lightReadString = test.substring(start,stop_val);
      int lightRead = lightReadString.toInt();
      //Serial.println(lightRead);
      lightOutput (lightRead);
    }
  }
  
  Serial.println(F(""));

  // CLOSE CONNECTION AND DISCONNECT
  www.close();
  Serial.println(F("Closing connection"));
  Serial.println(F(""));
} 

void lightOutput (int lightRead) {
  int lightReadMapped = map(lightRead, 6, 700, 0, 255);
  analogWrite(ledPin, lightReadMapped);
}


