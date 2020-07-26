#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <MQ135.h>
#include "DFRobot_PH.h" 

//DS18B20 declare###########################################
int sensorPinDS18b20 = 4; //GPO4 atau D2
float suhuC;
OneWire ourWire(sensorPinDS18b20);
DallasTemperature sensors(&ourWire);

//HC-SR04 declare###########################################
#define echoPin 11 //Echo Pin
#define trigPin 12 //Trigger Pin
int maximumRange = 200;   //kebutuhan akan maksimal range
int minimumRange = 00;    //kebutuhan akan minimal range
long duration, distance;  //waktu untuk kalkulasi jarak

//MQ-135 declare############################################
const int gasSensor = A1;

//pH_meter declare##########################################
const int phPin = 0;
#define sensor_ph A0
float tegangan,nilaiph;
DFRobot_PH ph;

void setup() {
  Serial.begin(115200); //BAUD RATE

  // HC-SR04###############################################
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // pH meter##############################################
    ph.begin(); //mulai untuk pembacaan sensor ph meter
    sensors.begin(); //mulai untuk pembacaan sensor ds18b20
  
}

void loop() {
  //DS18B20###############################################
  sensors.requestTemperatures();
  suhuC = sensors.getTempCByIndex(0);
  
  //HC-SR04###############################################
  digitalWrite(trigPin, LOW);delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  //perhitungan untuk dijadikan jarak
  distance = duration/58.2;

  //MQ-135################################################
  float voltage;
  voltage = getVoltage(gasSensor);

  //pH meter##############################################
  static unsigned long timepoint = millis();
  if(millis()-timepoint>1000U){                  //time interval: 1s
    timepoint = millis();
    tegangan = analogRead(sensor_ph)/1024.0*5000; //mengubah tegangan analog menjadi digital dan menjadi tegangan
    //suhuC = sensors.getTempCByIndex(00);
    nilaiph = ph.readPH(tegangan,suhuC);  //konversi tegangan menjadi ph meter dengan kompensasi suhuC
    //OUTPUT pH meter
    Serial.print("pH = ");
    Serial.print(nilaiph,2);
    Serial.println(" pH");
  }
  ph.calibration(tegangan,suhuC); 
  
  //OUTPUT
  //OUTPUT DS18B20
  Serial.print("DS18B20 = ");
  Serial.print(suhuC);
  Serial.print(" ");
  Serial.println("C");
  
  //OUTPUT HC-SR04
  if (distance >= maximumRange || distance <= minimumRange){
    Serial.print("HC-SR04 = ");
    Serial.println("-1");
  }
  else {
    /*Kirim jarak ke komputer menggunakan Serial protokol, dan
    menghidupkan LED OFF untuk menunjukkan membaca sukses. */
    Serial.print("HC-SR04 = ");
    Serial.print(distance);
    Serial.print(" ");
    Serial.println("cm");
  }

  //OUTPUT MQ-135
  Serial.print("MQ-135 = ");
  Serial.print(voltage);
  Serial.println(" PPM");
  
  //waktu tunda 50mS
  delay(5000);
}

float getVoltage(int pin){
  return (analogRead(pin) * 0.004882814);
  // This equation converts the 0 to 1023 value that analogRead()
  // returns, into a 0.0 to 5.0 value that is the true voltage
  // being read at that pin.
}
