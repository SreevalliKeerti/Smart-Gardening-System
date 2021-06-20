#define BLYNK_PRINT SwSerial

#include <SoftwareSerial.h>
SoftwareSerial SwSerial(10, 11); // RX, TX

#include <BlynkSimpleStream.h>

const int sensor_pin = A1;  /* Soil moisture sensor O/P pin */
int sensor_analog = 0; /*Moisture sensor value*/

const int trig = 2, echo = 3; /* Ultrasonic sensor trigger and echo pins */
int led = 13; /* LED on Uno board */
int waterpump = 4; /* Relay i/p pin */

char auth[] = "actual auth token goes here";

BlynkTimer timer;

boolean flag = false;

void sendSensor()
{
    float moisture_percentage;
    int sensor_analog;
    sensor_analog = analogRead(sensor_pin); /* Reading data from soil sensor */
    moisture_percentage = ( 100 - ( (sensor_analog/1023.00) * 100 ) ); /* Analog readings into percentage */

    long duration, Distanceincm;
    
    Serial.print("Moisture Percentage = ");
    Serial.print(moisture_percentage);
    Serial.print("%\n\n");
    
    if((moisture_percentage <= 30) && (flag==false)){ /* If the soil is dry */
      Serial.print("Water Pump: ON");
      Serial.print("\n\n");
      digitalWrite(led, HIGH);
      digitalWrite(waterpump, LOW); /* Activating relay */
      flag = true;
      delay(1000);
    }
    if((moisture_percentage >= 50) && (flag==true)){
      Serial.print("Water Pump: OFF");
      Serial.print("\n\n");
      digitalWrite(led, LOW);
      digitalWrite(waterpump, HIGH); /* Deactivating relay */
      flag = false;
      delay(1000);
    }
  
    
    digitalWrite(trig, HIGH);
    delay(15);
    digitalWrite(trig, LOW);
    duration = pulseIn(echo, HIGH);
    
    // The ultrasonic module is placed just above 30cm tank.
    Distanceincm = (duration / 58);
  
    long level = ((32 - Distanceincm)/32 * 100);
    
   // Blynk.virtualWrite(V5, sensor_analog);
    Blynk.virtualWrite(V6, moisture_percentage);
    Blynk.virtualWrite(V7, level);

    if (Distanceincm < 8) {  /* Above half level */
      digitalWrite(led, HIGH);
  
      // Serial.print("Good Water Level");
      // Serial.print("\n\n");
      delay(1000);
    }
    else {
      digitalWrite(led, LOW);  /* Below half level */
     
      Serial.print("Low Water Level!!");
      Serial.print("\n\n");
      delay(1000);
    }

     if (Distanceincm > 16){ /* Below quarter level */
  
      Serial.print("Water Pump: OFF");
      Serial.print("\n\n");
      
      digitalWrite(led, LOW);
      digitalWrite(waterpump, HIGH); /* Deactivating relay even if moisture percentage is low */
  
      delay(1000);
    }
    
    delay(2000);
    
    if(Distanceincm > 10) /* Alert notification to user */
    {
      Blynk.notify("Low Water Level");
    }
    
  
}

void setup()
{
  // Debug console
  SwSerial.begin(9600);

  
  Serial.begin(9600);
  Blynk.begin(Serial, auth);

  
  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}
