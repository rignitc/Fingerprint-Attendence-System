#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>
HardwareSerial serialPorty(2); // use UART2
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&serialPorty);
uint8_t getFingerprintID();
LiquidCrystal My_LCD(32,33, 25, 26, 27, 14);
#include "pitches.h"
#define BUZZZER_PIN  15 // ESP32 pin GPIO18 connected to piezo buzzer

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};
const char *names[] = {
  "Krishnaprasad",
  "Nikitha S",
  "Moushmi",
  "Sailaja Madasu",
  "Pranav Ramesh",
  "Aadhil. N",
  "Vishakha",
  "SNEHA JOE M",
  "Kathaan Vora",
  "Shibin Fazil",
  "Prudhvi",
  "Adhyuth Narayan",
  "Madhav Menon",
  "Aftaab Ahamed",
  "Karthik M",
  "Karthik Jayaraj",
  "Venkatsai",
  "Arnav Mishra",
  "Niraamay Dileep",
  "Rashith kp",
  "Indumathi M",
  "Prajul",
  "Krishnachandran",
  "Nithin",
  "Dogga Vamsidathu",
  "Piyush kumar",
  "Sri Nidhi",
  "Jerin Shaji",
  "Divit Reddy",
  "Jomal Joy",
  "Rajvardhan"
};

void setup()
{
    pinMode(BUZZZER_PIN, OUTPUT); 
    Serial.begin(115200);
    finger.begin(57600);
    My_LCD.begin(16, 2);
    My_LCD.clear();
      while (!Serial)
        ; // For Yun/Leo/Micro/Zero/...
    delay(100);
    Serial.println("\n\nAdafruit finger detect test");
    My_LCD.print("Diagnosing :)");
    delay(5);
    int p;
    while(p)
    {
    if (finger.verifyPassword())
    {
    Serial.println("Found fingerprint sensor!");
    My_LCD.clear();
    My_LCD.setCursor(0, 0);
    My_LCD.print("Connected :)");
    p=0;
    }
    else
    {
     Serial.println("Did not find fingerprint sensor :(");
     My_LCD.clear();
     My_LCD.setCursor(0, 0);
     My_LCD.print("Connection Fail:)");
     p=1;   
    }
    }
      for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZZER_PIN, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZZER_PIN);
  }
    
    Serial.println(F("Reading sensor parameters"));
    finger.getParameters();
    finger.getTemplateCount();

    if (finger.templateCount == 0)
    {
        Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
    }
    else
    {
        Serial.println("Waiting for valid finger...");
          My_LCD.clear();
          My_LCD.setCursor(0, 0);
          My_LCD.print("Welcome to RIG)");
        Serial.print("Sensor contains ");
        Serial.print(finger.templateCount);
        Serial.println(" templates");
    }
}

void loop()
{
    getFingerprintID();
    delay(50); // don't ned to run this at full speed.
}

uint8_t getFingerprintID()
{
    uint8_t p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Image taken");
               break;
    case FINGERPRINT_NOFINGER:
        Serial.println("No finger detected");
         return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
          My_LCD.clear();
    My_LCD.setCursor(0, 0);
    My_LCD.print("Comm Error 1");
        return p;
    case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
         My_LCD.clear();
    My_LCD.setCursor(0, 0);
    My_LCD.print("Img Error 1");
        return p;
    default:
        Serial.println("Unknown error");
         My_LCD.clear();
    My_LCD.setCursor(0, 0);
    My_LCD.print("Uk Error 1");
        return p;
    }

    // OK success!

    p = finger.image2Tz();
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
         return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
         My_LCD.clear();
    My_LCD.setCursor(0, 0);
    My_LCD.print("Comm Error 2");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        
        return p;
    default:
        Serial.println("Unknown error");
         My_LCD.clear();
    My_LCD.setCursor(0, 0);
    My_LCD.print("Uk Error 2");
        return p;
    }

    // OK converted!
    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Found a print match!");
         My_LCD.clear();
    My_LCD.setCursor(0, 0);
    My_LCD.print("Found a match:");
    delay(50);
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication error");
         My_LCD.clear();
    My_LCD.setCursor(0, 0);
    My_LCD.print("Comm Error 3");
        return p;
    }
    else if (p == FINGERPRINT_NOTFOUND)
    {
        Serial.println("Did not find a match");
    My_LCD.setCursor(0, 1);
    My_LCD.print("No match Found:");
    delay(500);
    My_LCD.setCursor(0, 1);
    My_LCD.print("                 ");
    return p;
    }
    else
    {
        Serial.println("Unknown error");
         return p;
    }

    // found a match!
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    My_LCD.setCursor(0, 1);
    //My_LCD.print("ID:");
    //My_LCD.setCursor(5,1);
    //My_LCD.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    for (int i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
    if (finger.fingerID == i + 1) {
     My_LCD.print(names[i]);
      //Serial.print(names[i]);
      break;
    }
  }
     digitalWrite(BUZZZER_PIN, HIGH);
    // Wait for 500 milliseconds (0.5 seconds)
  delay(500);
    // Turn off the buzzer
  digitalWrite(BUZZZER_PIN, LOW);
    // Wait for 500 milliseconds (0.5 seconds)
    delay(1500);
    My_LCD.clear();
    My_LCD.setCursor(0, 0);
    My_LCD.print("Welcome to RIG:)");
    

    return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez()
{
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK)
        return -1;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK)
        return -1;

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK)
        return -1;

    // found a match!
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    return finger.fingerID;
}
