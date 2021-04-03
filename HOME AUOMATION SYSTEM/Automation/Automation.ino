//pins for arrays serial.read()
#include <RTClib.h>
RTC_DS1307 rtc;


const int a=13;
const int b=12;
const int c=11;
const int d=10;
int moda=LOW;
int modb=LOW;
int modc=LOW;
int modd=LOW;
//int currentt=1;

//arrays to hold values of working modes and data for relays
// 1. order mode=0,1,2,3 0- On/OFF switch 1- Time based periodic, 2- Use sensor(temp), 3- selected time period
// 2. if mode==0 0- switch it off 1 switch it on 
// 2. if mode==1 start time in hhmm order
// 2. if mode==2 what should do right now turned on or off? 1-on 0-off
// 2. if mode==3 we will give current time and add the selected time duration
// 3. start time in hhmm 24hour (only for mode 2)
// 4. off time in hhmm 24hour   (only for mode 2)

int ar[4]={0,1,0,0};
int br[4]={1,1323,1324,0};
int cr[4]={0,1,0,0};
int dr[4]={0,0,0,0};

//const int tso={27,15};  // this is the temparetre to auto on and off **update we dont need an extra space to store 2 const 

int reading={33}; // Read from sensor current temprature

int blu[5]={0,0,1,0,0};  // pin , mode , on/off/start time, turn off time  This will be reciving from the blutooth module.

int currenttime=1200;  // current time read by the RTC module is 11:33 AM , need to implimaent a function for it.

int myrelayind(int ric[4]){ // this will analyis the array to get the right output mode we give ric=ar,br,cr,dr to this.
  
  if (ric[0]==0){  //if mode is 0 mode
   
    if (ric[1]==1){
      return HIGH;
      }
      else{
        return LOW;
        }
  }
  else if (ric[0]==1){ /// periodic mode
    Serial.println("In periodic");
    if (currenttime>=ric[1] and currenttime<=ric[2]){
      //Serial.println("make it high");
      return HIGH;

    }
    else{
      Serial.println("dafuq");
      return LOW;
    }
    }
  else if (ric[0]==2){  ///sensor mode
    if(reading>27){
      return HIGH;  
    }
    else if (reading<15){
      return LOW;
    }
  }
  else{  //// limited time mode
    if(currenttime>=ric[1]){
      return HIGH;
    }
    else{
      return LOW;
      
    } 
  }
  }
// we can reduce this code becosue the outstat is originaly 0, so no need else statments to make it 0 1% usage on chip though
int updateRelay(int blu_data[5]){ // take blutooth data and create a sliced array (do not know a better way to slice) output
  int emp_ar[4];                  // will replace current array data accoring to blu_data[0] which is actually blu[]  
  emp_ar[0]=blu_data[1];
  emp_ar[1]=blu_data[2];
  emp_ar[2]=blu_data[3];
  emp_ar[3]=blu_data[4];  
  return emp_ar;
}



void setup() {
  // put your setup code here, to run once:
  pinMode(a,OUTPUT);
  pinMode(b,OUTPUT);
  pinMode(c,OUTPUT);
  pinMode(d,OUTPUT);
  Serial.begin(9600);
// add blutooth, RTC, temp sensor input pins and the display output

}

// find a way to trigger updateRelay function when an update from blutooth module come,, will waste inside loop
// define a clock function to read from clock or someting as well as one for temp sensor
void loop() {
  DateTime now = rtc.now();
  
  currenttime=(now.hour()*100)+(now.minute());
  Serial.println(currenttime);

  
// Serial.print(now.hour(), DEC);
//  Serial.print(':');
//  Serial.print(now.minute(), DEC);
//  Serial.print(':');
//  Serial.print(now.second(), DEC);
//  Serial.println();
  moda=myrelayind(ar);
  modb=myrelayind(br);
  modc=myrelayind(cr);
  modd=myrelayind(dr);
  digitalWrite(a,moda);//myrelayind(ar));
  digitalWrite(b,modb);
  digitalWrite(c,modc);
  digitalWrite(d,modd);
  delay(1000);// we can use 1 minute but some where around 30-20 seconds is acceptable
}
