//***********************CC3000 Libraries
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
//#include <avr/wdt.h>


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

const int micPin = A0;
int soundLevel;     

long previousMillis = 0;  
long interval = 2000;    // interval at which to reset (milliseconds)
unsigned long currentMillis = 0;

uint32_t ip;

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
  Send();
  previousMillis = currentMillis; // save the last time you reset
  } 
}


void Send() {
  
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

  // SEND HTTP POST REQUEST
  Adafruit_CC3000_Client client = cc3000.connectTCP(ip, 80);
  if (client.connected()) {
    sensorRead();
    client.fastrprint(F("GET "));
    client.fastrprint(".../api/post/");
    client.print(soundLevel);
    client.fastrprint(F(" HTTP/1.1\r\n"));
    client.fastrprint(F("Host: ")); client.fastrprint("YOUR HOST"); client.fastrprint(F("\r\n"));
    client.fastrprint(F("\r\n"));
    client.println();
   }

  else {
  Serial.println(F("Connection failed"));    
  return;
  }

  // READ ANSWER FROM OUR SERVER FOR CONFIRMATION
  unsigned long lastRead = millis();
  Serial.println(F("Reading answer..."));
  while (client.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
      lastRead = millis();
    }
  }
  Serial.println(F(""));

  // CLOSE CONNECTION AND DISCONNECT
  client.close();
  Serial.println(F("Closing connection"));
  Serial.println(F(""));
} 
  
 

void sensorRead() {
  int mn = 1024; // mn only decreases
  int mx = 0;    // mx only increases
  

  // Perform 10000 reads. Update mn and mx for each one.
  for (int i = 0; i < 1000; ++i) {
    int val = analogRead(micPin);
    
    mn = min(mn, val);
    mx = max(mx, val);
    soundLevel = mx-mn;

  }
    Serial.print(" Sound=");
    Serial.println(soundLevel);
}
