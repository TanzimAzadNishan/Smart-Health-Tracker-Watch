#include <SoftwareSerial.h>
SoftwareSerial SIM900A(10,11);

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;


String ds18b20_temp = "";
String dht11_temp_main = "";
String dht11_temp_fraction = "";
String dht11_hum_main = "";
String dht11_hum_fraction = "";
String pulse_bpm = "";
String smsMessage;
int alreadyReceived = 0;
Vector rawAccel;
Vector normAccel;

void setup()
{
  SIM900A.begin(115200);   // Setting the baud rate of GSM Module  
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  Serial.println ("SIM900A Ready");
  Serial.println("Initialize MPU6050");

  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  
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
  if (Serial.available()>0 && alreadyReceived == 0)
  {
    pulse_bpm = Serial.readString();
    Serial.print("pulse_bpm: ");
    Serial.print(pulse_bpm);
    Serial.println("");
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


    rawAccel = mpu.readRawAccel();
    normAccel = mpu.readNormalizeAccel();    

    smsMessage = "Temperature: " + dht11_temp_main + "." + dht11_temp_fraction + "C\n";
    smsMessage = smsMessage + "Humidity: " + dht11_hum_main + "." + dht11_hum_fraction + "%\n";
    smsMessage = smsMessage + "Body Temperature: " + ds18b20_temp + "C\n";
    smsMessage = smsMessage + "Pulse Rate: " + pulse_bpm + " bpm\n";
    smsMessage = smsMessage + "Xraw = " + rawAccel.XAxis + ", Xnorm = " + normAccel.XAxis + "\n";
    smsMessage = smsMessage + "Yraw = " + rawAccel.YAxis + ", Ynorm = " + normAccel.YAxis + "\n";
    smsMessage = smsMessage + "Zraw = " + rawAccel.ZAxis + ", Znorm = " + normAccel.ZAxis + "\n";

    Serial.println("\n");
    Serial.println(smsMessage);
    //SendMessage();    
  }

  /*if (SIM900A.available()>0){
   Serial.write(SIM900A.read());
  }*/
  
}
