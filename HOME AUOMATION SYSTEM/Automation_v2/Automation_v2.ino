
int realaydata[4][4]={{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}; //a,b,c,d relay data (10,11,12,13 ports reserved)
int statedata[4]={LOW,LOW,LOW,LOW}; // realay a,b,c,d states

#include <RTClib.h>// add the RTC module, the pins are 4,3 resererved
RTC_DS1307 rtc; // the pins are defined inside
#include <dht11.h>
#define DHT11PIN 7 // pin 7 reserved

dht11 DHT11;


char * bluetoothData;
float temp;
float humid;
int timeNow;

int myrelayind(int ric[4]){ // Using Time. blutooth input and sensor data, will calculate Weather to turn on or off the realy
  
  if (ric[0]==0){  //if mode is 0 mode
    if (ric[1]==1){return HIGH;}
      else{return LOW;}
  }
  
  else if (ric[0]==1){ /// periodic mode
    if (timeNow>=ric[1] and timeNow<=ric[2]){return HIGH;}
    else{return LOW;}
    }
    
  else if (ric[0]==2){  ///sensor mode ldr will be added later.
    if(temp>27){return HIGH;}
    else if (temp<15){return LOW;}
  }
  
  else{  //// limited time mode
    if(timeNow>=ric[1] and timeNow<=ric[2]){return HIGH;}
    else{
      //write a code to reset the array so it wont repeat
      return LOW;} 
  }
  
  }

int Now(){ //get time in hhmm format
  DateTime now = rtc.now();
  timeNow=(now.hour()*100)+(now.minute());
  return timeNow;
  }


void blu_proc(){ // get raw data from bluetooth module
  while (Serial.available()){
    delay(10);
    bluetoothData += char(Serial.read());
    
    }
    Serial.println(bluetoothData);
  
  }

void relay_update(){ // bluetoothData looks like 2,1,1,2359,2359
  char * piece= strtok(bluetoothData,",");
  int relay=atoi(piece);//First chunk is the port
  realaydata[relay][0]=atoi(strtok(NULL,","));//set mode
  realaydata[relay][1]=atoi(strtok(NULL,","));
  realaydata[relay][2]=atoi(strtok(NULL,","));
  realaydata[relay][3]=atoi(strtok(NULL,","));
  }
  
void Temp_hum(){
  int chk = DHT11.read(DHT11PIN);
  humid=(float)DHT11.humidity;
  temp=(float)DHT11.temperature;
  //Serial.println("humidity");
  //Serial.println(humid);
  //Serial.println("Temp");
  //Serial.println(temp);
  
  }
  
void setup() {
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  timeNow=Now();
  //Serial.println(timeNow);
  while (Serial.available()){
    Serial.println("Its here");
    blu_proc();//Get bluetooth input
    relay_update();//update the realy using input data
    bluetoothData="";// reset storeage
    }
  
  Temp_hum();
  
  for (unsigned int a = 0; a < 4; a = a + 1){
    int p=a+10; // port
    
    int stat=myrelayind(realaydata[a]);//High or LOW value for
    statedata[a]=stat;// this is redundent for now, but will be useful for display
    digitalWrite(p,stat);
    }
  //delay();
     
}
