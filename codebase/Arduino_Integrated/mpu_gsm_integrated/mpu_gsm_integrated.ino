#include <LiquidCrystal.h>

#include <SoftwareSerial.h>
SoftwareSerial SIM900A(10,11);
//LiquidCrystal lcd(13,12,5,4,3,2);

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
float dx=0.0f;
float vx=0.0f;
float dt = 0.005;
unsigned long startTime;
unsigned long curTime;
float acc_xp=0.0f;
float acc_xc=0.0f;
float ux = 0.0f;
float ax = 0.0f;

float acceleration_x[100];
float acceleration_y[100];
float acceleration_z[100];
/*float dy=0.0f;
float vy=0.0f;
float dz=0.0f;
float vz=0.0f;*/

float acc_xpv;
int delay_cnt = 0;
int footstep = 0;
int fall_delay_cnt = 0;
bool isRotated = false;
int stableCount = 0;
bool isFall = false;
String smsMessage;


void setup(void) {
  SIM900A.begin(115200); 
  Serial.begin(9600);
  //lcd.begin(16,2);
  //lcd.print("hello nigga!");
  smsMessage = smsMessage + "Fallout detected!!!!!";

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

void loop() {
  //lcd.write("fgxgx");
  //lcd.setCursor(1, 0);
  //lcd.print("hcsd");

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  delay(100);
  Serial.println(a.acceleration.z, 6);

   if(stableCount < 10){
      stableCount++; 
   }
 
   if(stableCount == 10){
       if(a.acceleration.z < -5.0){
           isRotated = true;
           fall_delay_cnt++;
           if(fall_delay_cnt % 10 == 0){
              Serial.println("Fall!");
              SendMessage();
           }
       }
       else{
          isRotated = false; 
       } 
    
       if(!isRotated){
         if(fall_delay_cnt % 5 == 0 && (a.acceleration.z < 7.0)){
             fall_delay_cnt++;
         }
         else if(fall_delay_cnt % 5 != 0 && (a.acceleration.z < 7.0)){
            fall_delay_cnt++;
            if(fall_delay_cnt % 5 == 0){
              isFall = true;
              Serial.println("Fall!");
              SendMessage();
            }
         } 
         else if(isFall && (a.acceleration.z > 11 || a.acceleration.z < 10)){
            isFall = false; 
         } 
         else if(!isFall && delay_cnt%15 == 0 && (a.acceleration.z > 11 || a.acceleration.z < 10)){

            fall_delay_cnt = 0;
            delay_cnt++;
            footstep++;
            Serial.print("footstep = ");
            Serial.println(footstep);      
         }
         else if(!isFall && delay_cnt%15 == 0 && (a.acceleration.z <= 11 && a.acceleration.z >= 10))
         {
          fall_delay_cnt = 0;
         }
         else{
            fall_delay_cnt = 0;
            delay_cnt++;
         }
       }
   }
}
