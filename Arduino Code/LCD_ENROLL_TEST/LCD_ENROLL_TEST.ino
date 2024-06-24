#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>
HardwareSerial serialPort(2); // use UART2
LiquidCrystal My_LCD(32,33, 25, 26, 27, 14);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&serialPort);

uint8_t id;
uint8_t getFingerprintEnroll();
uint8_t getFingerprintID();
#define BUZZZER_PIN  15 // ESP32 pin GPIO18 connected to piezo buzzer
void setup()
{ My_LCD.begin(16, 2);
  pinMode(BUZZZER_PIN, OUTPUT);
  // Clears The LCD Display
  My_LCD.clear();
 
  Serial.begin(115200);
  while (!Serial)
    ; // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword())
  {
    Serial.println("Sucessfully Connected:");
      My_LCD.print("Connected:)");
  }
  else
  {
    Serial.println("Connection Error:");
    My_LCD.print("Connection Failed:)");
    while (1)
    {
      delay(1);
    }
  }

  //Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  //Serial.print(F("Status: 0x"));
  //Serial.println(finger.status_reg, HEX);
  //Serial.print(F("Sys ID: 0x"));
  //Serial.println(finger.system_id, HEX);
  //Serial.print(F("Capacity: "));
  //Serial.println(finger.capacity);
  //Serial.print(F("Security level: "));
  //Serial.println(finger.security_level);
  //Serial.print(F("Device address: "));
  //Serial.println(finger.device_addr, HEX);
  //Serial.print(F("Packet len: "));
  //Serial.println(finger.packet_len);
  //Serial.print(F("Baud rate: "));
  //Serial.println(finger.baud_rate);
}

uint8_t readnumber(void)
{
  uint8_t num = 0;

  while (num == 0)
  {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
  finger.getTemplateCount();
}
void functionA();
void functionB();
void loop() // run over and over again
{

        functionA();
        
        functionB();
        
    }
  

  
void functionA(){
  
  Serial.println("Ready to enroll");
  My_LCD.print("Enrollment :)");
  Serial.println("Please type ID#1 to 127");
  My_LCD.clear();
  My_LCD.print("Enter ID");
  id = readnumber();
  if (id == 0)
  { // ID #0 not allowed, try again!
    return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);

  while (!getFingerprintEnroll())
    ;
}
void functionB() {
    getFingerprintID();
    delay(50); // don't ned to run this at full speed.
}



uint8_t getFingerprintEnroll()
{

  int p = -1;
  Serial.print("Keep your finger to enroll as #");
  My_LCD.clear();
  My_LCD.print("Keep your Finger");
  
  Serial.println(id);
  delay(2000);
  My_LCD.setCursor(0, 1);
   My_LCD.print("Please Wait");
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      My_LCD.clear();
      My_LCD.print("Comm Error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      My_LCD.clear();
      My_LCD.print("Img Error");
      break;
    default:
      Serial.println("Unknown error");
      My_LCD.clear();
      My_LCD.print("Error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
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
      My_LCD.print("Error");
    return p;
  case FINGERPRINT_FEATUREFAIL:
    Serial.println("Could not find fingerprint features");
    return p;
  case FINGERPRINT_INVALIDIMAGE:
    Serial.println("Could not find fingerprint features");
    return p;
  default:
    Serial.println("Unknown error");
    return p;
  }

  Serial.println("Remove finger");
    My_LCD.clear();
  My_LCD.print("Remove finger and");
    My_LCD.setCursor(0, 1);
    My_LCD.print("keep once again");
  delay(2000);
    My_LCD.clear();
     My_LCD.print("Please Wait");

  p = 0;
  while (p != FINGERPRINT_NOFINGER)
  {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  delay(1500);
  Serial.println("Place same finger again");
  delay(2000);
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      My_LCD.clear();
      My_LCD.print("Error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      My_LCD.clear();
      My_LCD.print("Error");
      break;
    default:
      Serial.println("Unknown error");
      My_LCD.clear();
      My_LCD.print("Error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p)
  {
  case FINGERPRINT_OK:
    Serial.println("Image converted");
    break;
  case FINGERPRINT_IMAGEMESS:
    Serial.println("Image too messy");
    My_LCD.clear();
      My_LCD.print("Error");
    return p;
  case FINGERPRINT_PACKETRECIEVEERR:
    Serial.println("Communication error");
    My_LCD.clear();
      My_LCD.print("Error");
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
      My_LCD.print("Error");
    return p;
  }

  // OK converted!
  Serial.print("Please Wait");
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK)
  {
    Serial.println("Prints matched!");
    My_LCD.clear();
      My_LCD.print("Prints Matched");
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    Serial.println("Communication error");
    return p;
  }
  else if (p == FINGERPRINT_ENROLLMISMATCH)
  {
    Serial.println("Fingerprints did not match");
    My_LCD.clear();
      My_LCD.print("Prints not match");
      delay(500);
    return p;
  }
  else
  {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK)
  {
    Serial.println("Successful");
    My_LCD.clear();
      My_LCD.print("Successfull");
       My_LCD.setCursor(0, 1);
          My_LCD.print("ID:");
           My_LCD.setCursor(0,5);
             My_LCD.print(id);
      delay(1000);
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    Serial.println("Communication error");
    return p;
  }
  else if (p == FINGERPRINT_BADLOCATION)
  {
    Serial.println("Failed");
    return p;
  }
  else if (p == FINGERPRINT_FLASHERR)
  {
    Serial.println("Error writing to flash");
     My_LCD.clear();
      My_LCD.print("Flash error");
      delay(1000);
    return p;
  }
  else
  {
    Serial.println("Unknown error");
    return p;
  }

  return true;
 
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
        return p;
    case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        return p;
    default:
        Serial.println("Unknown error");
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
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
    default:
        Serial.println("Unknown error");
        return p;
    }

    // OK converted!
    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Found a print match!");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication error");
        return p;
    }
    else if (p == FINGERPRINT_NOTFOUND)
    {
        Serial.println("Did not find a match");
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
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);

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
