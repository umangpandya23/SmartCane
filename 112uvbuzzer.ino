#include <SoftwareSerial.h>
#include <IRremote.h>
SoftwareSerial mySerial(4, 5);

#define trigPin1 12
#define echoPin1 13
#define buzzer 9
#define trigPin2 11
#define echoPin2 10
#define led 8
#define trigPin3 7
#define echoPin3 6
#define buttonpin 3

int i=0,k=0;
int  gps_status=0;
String cordinat="<p>Coordinates:</p>";
String latitude=""; 
String logitude=""; 
String gpsString="";
char *test="$GPGGA";            //declared for receiving pulses

int RECV_PIN = 2;
int c=0;                        // declared if you want to control with a single button
IRrecv irrecv(RECV_PIN);
decode_results results;

int LDR = 0;
int LDRvalue = 0;
int light_sensitivity = 500;
long duration1,duration2,duration3;
int safetyDistance,distance1,distance2,distance3;

void setup() { 
  Serial.begin(9600);
  mySerial.begin(9600);// Setting the baud rate of GSM Module    
  pinMode(buttonpin,INPUT); 
  pinMode(trigPin1,OUTPUT);
  pinMode(echoPin1,INPUT);
  pinMode(trigPin2,OUTPUT);
  pinMode(echoPin2,INPUT);
  pinMode(trigPin3,OUTPUT);
  pinMode(echoPin3,INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led,OUTPUT);
  irrecv.enableIRIn();
  Serial.println("press button");
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value);
    irrecv.resume();                        // Receive the next value
    if((results.value== 33441975)&&(c%2==1))                    // enables you to turn on & off with the help of same button"                  
    {
                digitalWrite(9,HIGH);
                Serial.println("Stick calling");
                delay(1000);
    }
    else if((results.value==33441975)&&(c%2==0))
    {
                digitalWrite(9,LOW);
    }
    c++;
  }
  
    Serial.println("Press key:");
    delay(1000);
    if(digitalRead(buttonpin)==HIGH)
    {
      
      Serial.println("button pressed");
      delay(2000);
      SendMessage();  
    }

    if (mySerial.available()>0)
    Serial.write(mySerial.read());

    /*if (irrecv.decode(&results)) {
      Serial.println(results.value);
      irrecv.resume();                        // Receive the next value
      if((results.value== 33441975)&&(c%2==1))                    // enables you to turn on & off with the help of same button"                  
      {
                digitalWrite(9,HIGH);
      }
      else if((results.value==33441975)&&(c%2==0))
      {
                  digitalWrite(9,LOW);
      }
      c++;
    }*/
 //for distance, pit and stairs calculation
  digitalWrite(trigPin1,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1,HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin1,LOW);
  duration1=pulseIn(echoPin1,HIGH);

    
  digitalWrite(trigPin2,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2,HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin2,LOW);
  duration2=pulseIn(echoPin2,HIGH);

  digitalWrite(trigPin3,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin3,HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin3,LOW);
  duration3=pulseIn(echoPin3,HIGH);
  
  distance1=(duration1/2)*0.0343;
  distance2=(duration2/2)*0.0343;
  distance3=(duration3/2)*0.0343;
  safetyDistance=distance1/distance2;
  
  Serial.print("Distance: ");
  
  if(safetyDistance >=2 && safetyDistance <=4){
    Serial.println("STAIRS ALERT.");
    digitalWrite(buzzer, HIGH);
  }
  else if(distance3 > 75){
    Serial.println("pit alert.");
    digitalWrite(buzzer,HIGH);
  }
  else if(distance2 < 30){
    Serial.println("small stone.");
    digitalWrite(buzzer,HIGH);
  }
  else{
    Serial.print(distance1);
    Serial.println(" cm");
    Serial.print(distance2);
    Serial.println(" cm");
    Serial.print(distance3);
    Serial.println(" cm");
    Serial.print(safetyDistance);
    Serial.println(" ratio");
    digitalWrite(buzzer, LOW);
  }

  //for light intensity checking
  LDRvalue = analogRead(LDR);
  Serial.println(LDRvalue);
  delay(50);

  if(LDRvalue < light_sensitivity){
     digitalWrite(led,HIGH);
     Serial.println("light low");
     digitalWrite(buzzer, HIGH);
  }
  else{
    digitalWrite(led,LOW);
     Serial.println("light high");
  }
}

 
void SendMessage()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second

  mySerial.println("AT+CMGS=\"+919825693698\"\r"); // Replace x with mobile number
  delay(1000);
  x=coordinate2dec();
  mySerial.println(x);// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

void coordinate2dec()
{
    
    //j=0;
    String lat_degree="";
    for(i=18;i<20;i++)          //extract latitude from string
      lat_degree+=gpsString[i];
    
    String lat_minut="";
    for(i=20;i<28;i++)
      lat_minut+=gpsString[i];
    
    String long_degree="";
    for(i=30;i<33;i++)          //extract longitude from string
      long_degree+=gpsString[i];
      
    String long_minut="";
    for(i=33;i<41;i++)
      long_minut+=gpsString[i];
      
     float minut= lat_minut.toFloat();
     minut=minut/60;
     float degree=lat_degree.toFloat();
     latitude=degree+minut;
     
     minut= long_minut.toFloat();
     minut=minut/60;
     degree=long_degree.toFloat();
     logitude=degree+minut;
}

/*void Send()
{
     
      webpage = "<h1>Welcome to Saddam Khan's Page</h1><body bgcolor=f0f0f0>";
      webpage+=name;
      webpage+=dob;
      webpage+=number;
      webpage+=cordinat;
      webpage+="<p>Latitude:";
      webpage+=latitude;
      webpage+="</p>";
      webpage+="<p>Longitude:";
      webpage+=logitude;
      webpage+="</p>";
     webpage+= "<a href=\"http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
     webpage+=latitude;
     webpage+='+';              //28.612953, 77.231545   //28.612953,77.2293563
     webpage+=logitude;
     webpage+="\">Click Here for google map</a>";
     sendwebdata();
       webpage="";

     while(1)
     {
      Serial.println("AT+CIPCLOSE=0");
      Serial1.println("AT+CIPCLOSE=0");
      while(Serial1.available())
      {
        //Serial.print(Serial1.read());
        if(Serial1.find("0,CLOSE"))
        {
          return;
        }
      }
      delay(500);
      i++;
      if(i>5)
      {
        i=0;
      }
      if(i==0)
      break;
     }
}

void sendwebdata()
{
     i=0;
     while(1)
     {
      unsigned int l=webpage.length();
      Serial1.print("AT+CIPSEND=0,");
      Serial1.println(l+2);
      Serial.println(l+2);
      Serial.println(webpage);

      Serial1.println(webpage);
      while(Serial1.available())
      {
        if(Serial1.find("OK"))
        {
         return;
        }
      }
            i++;
      if(i>5)
        i=0;
        if(i==0)
        break;
      delay(200);
     }
}
*/
