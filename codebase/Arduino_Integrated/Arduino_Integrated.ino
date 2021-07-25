#include <SoftwareSerial.h>
SoftwareSerial SIM900A(10,11);

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#include <Wire.h>

Adafruit_MPU6050 mpu;
int delay_cnt = 0;
int footstep = 0;

String ds18b20_temp = "";
String dht11_temp_main = "";
String dht11_temp_fraction = "";
String dht11_hum_main = "";
String dht11_hum_fraction = "";
String pulse_bpm = "";
String smsMessage;
int alreadyReceived = 0;
bool isReadyForSms = false;

void setup()
{
  SIM900A.begin(115200);   // Setting the baud rate of GSM Module  
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  Serial.println ("SIM900A Ready");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

    // set accelerometer range to +-8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  // set gyro range to +- 500 deg/s
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  // set filter bandwidth to 21 Hz
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);
  
  pinMode(0,INPUT);
  pinMode(1,OUTPUT);
  Serial.println ("Transferring data from ATMega32 to Arduino...");
}


void SendMessage()
{
  SIM900A.println("AT");
  Serial.println ("Sending Message");
  SIM900A.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  Serial.println ("Set SMS Number");
  SIM900A.println("AT+CMGS=\"+8801724729159\"\r"); //Mobile phone number to send message
  delay(1000);
  Serial.println ("Set SMS Content");
  SIM900A.println(smsMessage);// Messsage content
  delay(100);
  Serial.println ("Finish");
  SIM900A.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  Serial.println ("Message has been sent");
}


void loop()
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  delay(250);
  Serial.println(a.acceleration.z, 6);

  if(footstep % 10 == 0 && isReadyForSms){
    isReadyForSms = false;
    smsMessage = smsMessage + "Footstep: " + footstep;

    //Serial.println("\n");
    Serial.println(smsMessage);
    SendMessage();
  }

  if(delay_cnt%5 == 0 && (a.acceleration.z > 11.0 || a.acceleration.z < 10.3)){
      delay_cnt++;
      footstep++;
      Serial.print("footstep = ");
      Serial.println(footstep);

      if(footstep % 10 == 0){
        isReadyForSms = true; 
      }
   }
   else if(delay_cnt%5 == 0 && (a.acceleration.z <= 11.0 && a.acceleration.z >= 10.3)){}
   else{
      delay_cnt++;
   }

  
  /*if (Serial.available()>0 && alreadyReceived == 0)
  {
    pulse_bpm = Serial.readString();
    Serial.print("pulse_bpm: ");
    Serial.print(pulse_bpm);
    Serial.println("");

    //smsMessage = Serial.readString();
    //Serial.println(smsMessage);    
    alreadyReceived = 1;

    dht11_temp_main = Serial.readString();
    Serial.print("dht11_temp_main: ");
    Serial.print(dht11_temp_main);
    Serial.println("");

    dht11_temp_fraction = Serial.readString();
    Serial.print("dht11_temp_fraction: ");
    Serial.print(dht11_temp_fraction);
    Serial.println("");

    dht11_hum_main = Serial.readString();
    Serial.print("dht11_hum_main: ");
    Serial.print(dht11_hum_main);
    Serial.println("");

    dht11_hum_fraction = Serial.readString();
    Serial.print("dht11_hum_fraction: ");
    Serial.print(dht11_hum_fraction);
    Serial.println("");

    ds18b20_temp = Serial.readString();
    Serial.print("ds18b20_temp: ");
    Serial.print(ds18b20_temp);
    Serial.println("");    

    smsMessage = "Temperature: " + dht11_temp_main + "." + dht11_temp_fraction + "C\n";
    smsMessage = smsMessage + "Humidity: " + dht11_hum_main + "." + dht11_hum_fraction + "%\n";
    smsMessage = smsMessage + "Body Temperature: " + ds18b20_temp + "C\n";
    smsMessage = smsMessage + "Pulse Rate: " + pulse_bpm + " bpm\n";

    Serial.println("\n");
    Serial.println(smsMessage);
    //SendMessage();    
  }*/

  /*if (SIM900A.available()>0){
   Serial.write(SIM900A.read());
  }*/
  
}
