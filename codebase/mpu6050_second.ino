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


void setup(void) {
  Serial.begin(9600);

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

  for(int i=0; i < 100; i++){
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    acceleration_x[i] = a.acceleration.x;
    delay(500);
    Serial.print(i);
    Serial.print(" ");
    Serial.println(acceleration_x[i], 6); 
  }
  int cnt = 0;
  for(int i=0; i < 10; i++){
    float diff = 0;
    for(int j = 1; j < 10; j++){
        diff += acceleration_x[i*10+j] - acceleration_x[i*10+(j-1)];
    }
    diff = diff/10.0;
    if(abs(diff) > 0.04){
      cnt++;  
    }

    Serial.print(i);
    Serial.print(" ");
    Serial.println(diff, 6);     
    /*if(abs(acceleration_x[i-2]-acceleration_x[i]) > 1.5){
      Serial.println(i);
      cnt = cnt + 1;
      i = i + 2;  
    }
    else if(abs(acceleration_x[i-1]-acceleration_x[i]) > 1.5){
      Serial.println(i);
      cnt = cnt + 1;
      i = i + 1;  
    }*/
  }

  Serial.print("footstep = ");
  Serial.println(cnt);

  /*for(int i=0; i < 30; i++){
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    acceleration_x[i] = a.acceleration.x;
    acceleration_y[i] = a.acceleration.y;
    acceleration_z[i] = a.acceleration.z;
    delay(50); 
  }

  for(int i=1; i < 30; i++){
    vx+=(acceleration_x[i-1] + acceleration_x[i])/2.0f*dt;
    dx+=vx*dt;

    vy+=(acceleration_y[i-1] + acceleration_y[i])/2.0f*dt;
    dy+=vy*dt;

    vz+=(acceleration_z[i-1] + acceleration_z[i])/2.0f*dt;
    dz+=vz*dt;        
  }

  dx = sqrt(dx*dx + dy*dy + dz*dz);*/
  
  /* Get new sensor events with the readings */
   /*sensors_event_t a, g, temp;
   mpu.getEvent(&a, &g, &temp);
   acc_xp = a.acceleration.x;
   acc_xpv = acc_xp;  

   for (int i=1; i<10; i++)
   {
     startTime = millis();
     sensors_event_t a, g, temp;
     mpu.getEvent(&a, &g, &temp);
     acc_xc = a.acceleration.x;
     curTime = millis();
     delay(1000);

     dt = (curTime - startTime)/1000.0;
     ax = (acc_xc + acc_xp)/2.0;
     vx = vx + ax*dt;
     dx += vx * dt;

     acc_xp = acc_xc;
     ux = vx;
   }*/

  //if(dx > 0.01){
   // dx = dx * 10.0;  
 //}
  
  //Serial.print("displacement = ");
  //Serial.println(dx, 6);
}

void loop() {

  /* Print out the values */
  //Serial.print("Acceleration X: ");
  //Serial.print(a.acceleration.x);
  //Serial.print(", Y: ");
  //Serial.print(a.acceleration.y);
  //Serial.print(", Z: ");
  //Serial.print(a.acceleration.z);
  //Serial.println(" m/s^2");

  //Serial.print("Rotation X: ");
  //Serial.print(g.gyro.x);
  //Serial.print(", Y: ");
  //Serial.print(g.gyro.y);
  //Serial.print(", Z: ");
  //Serial.print(g.gyro.z);
  //Serial.println(" rad/s");

  //Serial.print("Temperature: ");
  //Serial.print(temp.temperature);
  //Serial.println(" degC");

  //Serial.println("");
  //delay(2000);
}
