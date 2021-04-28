
int realaydata[4][4]={{0,1,0,0},{0,1,0,0},{0,1,0,0},{5,14,25,0}}; //a,b,c,d relay data (10,11,12,13 ports reserved)
int statedata[4]={LOW,LOW,LOW,LOW}; // realay a,b,c,d states

#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68//A4,A5 reserved
#include <dht11.h>
#define DHT11PIN 7 // pin 7 reserved

dht11 DHT11;

String temper;
char * bluetoothData;
float temp;
float humid;
int timeNow;
int count[4]={0,0,0,0};

byte decToBcd(byte val){
  return( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val){
  return( (val/16*10) + (val%16) );
}

void setDS3231time( byte second, byte minute, byte hour){
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  //Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  //Wire.write(decToBcd(month)); // set month
  //Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void readDS3231time(byte *second,
byte *minute,
byte *hour)
//byte *dayOfMonth,
//byte *month,
//byte *year

{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 3);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  //*dayOfWeek = bcdToDec(Wire.read());
  //*dayOfMonth = bcdToDec(Wire.read());
  //*month = bcdToDec(Wire.read());
  //*year = bcdToDec(Wire.read());
}

int displayTime(){
  
  byte second,minute, hour;
  readDS3231time(&second,&minute, &hour);
  int tic=hour*100+minute;
  return tic;
  }

int myrelayind(int ric[4],int ind){ // Using Time. blutooth input and sensor data, will calculate Weather to turn on or off the realy
  
  if (ric[0]==0){  //if mode is 0 mode
    if (ric[1]==1){return HIGH;}
      else{return LOW;}
  }
  
  else if (ric[0]==1){ /// periodic mode
    if (timeNow>=ric[1] and timeNow<ric[2]){return HIGH;}
    else{return LOW;}
    }
    
  else if (ric[0]==2){  //Temparature Mode for Cooling
    if(temp>ric[1]){
      //Serial.println(temp);
      //Serial.println("HOT");
      return HIGH;}
    else if (temp<ric[2]){
      //Serial.println(temp);
      //Serial.println("COLD");
      return LOW;}
    else{

      if (statedata[ind]==LOW){
        return LOW;}
      else{
        return HIGH;
        }
        }
      
      }
  
  else if(ric[0]==3){  //// limited time mode
    if(timeNow>=ric[1] and timeNow<=ric[2]){return HIGH;}
    else{
      //write a code to reset the array so it wont repeat
      
      for(int i = 0; i < 4; i++) {
    
        realaydata[ind][i] = 0 ;  // put your value here.
        }
      return LOW;} 
  }
  
   else if(ric[0]==4){// Hourly Mode
    // find a proper algorithem
    int minu=timeNow%100;// get minutes
    if (ric[1]==0){ //run every hour permanently
      if(minu>=ric[2] and minu<ric[3]){return HIGH;}
      else{
        return LOW;}
      }
    else{
      if (count[ind]==ric[1]){ // the repetetion is over
        count[ind]=0;
        for(int i = 0; i < 4; i++){
          realaydata[ind][i] = 0 ;  // reset read data.
        }
        return LOW;// turn off
        }
        
      else if (count[ind]<ric[1]){// since we start using 0 it sould work
        if (minu>=ric[2] and minu<ric[3]){// check if it's in the proper time limit we assigned
          return HIGH;
          }
        else if (minu<ric[2] or minu>=ric[3]){ // not in the given limit 
          if (statedata[ind]==HIGH){// if it just got low the counter need to go up
            count[ind]=count[ind]+1;
            return LOW;
            }          
            }
        }
      else{return LOW;}
      }
    }
   else if (ric[0]==5){  // Temparature Mode for heating
    //Serial.println("We are at heating");
    if(temp<ric[1]){
      //Serial.println(temp);
      //Serial.println("Too cold");
      return HIGH;}
    else if (temp>ric[2]){
      //Serial.println(temp);
      //Serial.println("Going Too hot");
      return LOW;}
    else{
      if (statedata[ind]==LOW){return LOW;}
      else{return HIGH;}
        }
      }
     else if(ric[0]==8){ // humidity mode
      int mid=(ric[2]+ric[1])/2;
      if (humid>ric[2]){ // too much humid
        return HIGH;
      }
      
      else if(humid<ric[1]){// too dry
        return HIGH;
        }
       else if(humid==mid){// will turn off the device when the humidty in a proper value which is comfortable
        return LOW;
        }  
      
      }
  
  }


void blu_proc(){ // get raw data from bluetooth module
  temper=Serial.readString();
  bluetoothData=temper.c_str();
  }

void relay_update(){ // bluetoothData looks like 2,1,1,2359,2359
  char * piece= strtok(bluetoothData,",");
  int relay=atoi(piece);//First chunk is the port or incase of clock mode 7
  if (relay==7){// trigger time setting mode
    int clock_data[4];
    clock_data[0]=atoi(strtok(NULL,","));//set mode
    clock_data[1]=atoi(strtok(NULL,","));
    clock_data[2]=atoi(strtok(NULL,","));
    clock_data[3]=atoi(strtok(NULL,","));
    setDS3231time(clock_data[0],clock_data[1],clock_data[2]);
    }
  else{  
  realaydata[relay][0]=atoi(strtok(NULL,","));//set mode
  realaydata[relay][1]=atoi(strtok(NULL,","));
  realaydata[relay][2]=atoi(strtok(NULL,","));
  realaydata[relay][3]=atoi(strtok(NULL,","));}
  
  }
  
void Temp_hum(){
  int chk = DHT11.read(DHT11PIN);
  humid=(float)DHT11.humidity;
  temp=(float)DHT11.temperature;
  //Serial.println("humidity");
  //Serial.println(humid);
  //Serial.println("Temp");
  //Serial.println(temp);// write codes here to print on LCD 
  
  }
  
void setup() {
  Wire.begin();
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  Serial.begin(9600);
  setDS3231time(0,0,12);// a initial state for Now
}

void loop() {
  timeNow=displayTime();// take time
  //Serial.println(timeNow);
  if (Serial.available()>0){
    blu_proc();//Get bluetooth input
    relay_update();//update the realy using input data
    bluetoothData="";// reset storeage
    }
  
  Temp_hum();
  
  for (unsigned int a = 0; a < 4; a = a + 1){
    int p=a+10; // port for inconviance
    
    int stat=myrelayind(realaydata[a],a);//High or LOW value for
    statedata[a]=stat;// this is redundent for now, but will be useful for display
    digitalWrite(p,stat);
    }
  delay(1000);// delay can be increased but the clock will not work properly
}
