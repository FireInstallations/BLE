#include <SoftwareSerial.h>
SoftwareSerial mySerial(7,8); // RX, TX  
// Connect BT05      Arduino Uno
//     TXD          Pin 7
//     RXD          Pin 8
//     VCC          5V
//     Gnd          Gnd
//     Speaker      Pin 13 and to ground (Attention)
//     LED          Pin 6 (long leg) and 330 Ohm resistor to graound (Meditation)
/*  Provisions: Purchase from Sichiray China a mindwave BLE Headset for about 100$
 *              Purchase from Amazon  2 von Bobury BT05 A Bluetooth 4.0 BLE Modul Cc2541 Serial Modul Transparente Übertragung Ibeacon
                          Verkauf durch: bobury EUR 4,75
                Take an Arduino Uno with a shield, connect the BLE module as above and load the program
                open the Serial Monitor and check, if the baudrate is 9600 and as well NL and CR are activated
                Give the Command AT the response must be OK
                Give the Command AT+ROLE the response must be +ROLE0
                Give the Command AT+ROLE1 the response mus be +get1, the BT05 is in master mode and this choice is permanent also in power down
    Using:      Open the Serial Monitor as above, give AT, than AT+INQ, the BT05 searches for slaves, and than AT+CONN1, if it is the first module
                Than the Headsets sends data, to see the data in order, give on the monitor E
                To see the meditation and attention value give M
    S180127_BLE_autoconnect:  The AT commands are given automaitcally with no response on success beside the BT05 LED turns from blinking to permanent
                               as well as the Sichiray headset blue diode
                               When the headset is switched of, please switch it on again and press the reset button on the Arduino, than automatic
                               connection is repeated
                               When the connection is broken by switching off the headset and waiting for seconds, the AT commands can be given manually.
 */
#define NoTone 0
#define c1    65 
#define des1  69
#define d1    73
#define es1   77
#define e1    82
#define f1    87
#define ges1  92
#define g1    98
#define as1   103
#define a1    110
#define b1    116
#define h1    123
#define c2    130 
#define des2  138
#define d2    146
#define es2   155
#define e2    164
#define f2    174
#define ges2  185
#define g2    196
#define as2   207
#define a2    220
#define b2    231
#define h2    246
#define c3    261 
#define des3  277
#define d3    293
#define es3   311
#define e3    329
#define f3    349
#define ges3  369
#define g3    392
#define as3   415
#define a3    440
#define b3    466
#define h3    493
#define c4    523 
#define des4  554
#define d4    587
#define es4   622
#define e4    659
#define f4    698
#define ges4  739
#define g4    783
#define as4   830
#define a4    880
#define b4    932
#define h4    987
#define c5    1046
#define des5  1108

int toene[51] = {NoTone,c1,des1,d1,es1,e1,f1,ges1,g1,as1,a1,b1,h1,c2,des2,d2,es2,e2,f2,ges2,g2,as2,a2,b2,h2,c3,des3,d3,es3,e3,f3,ges3,g3,as3,a3,b3,h3,c4,des4,d4,es4,e4,f4,ges4,g4,as4,a4,b4,h4,c5,des5};

bool dataFlag = true, beginFlag = false,reduceFlag = true, atFlag = false, displayFlag = false;
double actualTime = 0, readTime = 0;
int sig = 200, attention = 0, meditation = 0, counter = 0, i = 0, elapse = 1;
long Wert[50];
byte data[50];
char c;
char d;
byte e = '0';
String Send ="";

void setup() {  
  Serial.begin(9600);
  mySerial.begin(9600);
  for(i = 1; i <= 50;i++){tone(13,toene[i]);delay(100);}
  delay(1000);
  noTone(13);
  autoconnect();
  c = 'M';// set M mode as default mode
  data[5] = 200;
  pinMode(6, OUTPUT);
  for (i = 1; i <= 50; i++) Wert[i] = 0;
 // Serial.println("automatic connection completed");//check the blue diode on Sichiray Headset permanently lid
}

void loop() {
  actualTime = millis();
  Send ="";// Send is the string containing all available data in one set and is used to be transmitted via serial interface, esp. to TWEDGE

  if (Serial.available()) {
    c = Serial.read();
    mySerial.print(c);}
    // decision, what has to be transmitted over serial interface
    if (c == 'X'){dataFlag = true;reduceFlag = false;atFlag = false;displayFlag = false;} //print Bytes
    if (c == 'A'){dataFlag = false;reduceFlag = false;atFlag = true;displayFlag = false;} //AT commands + Direct read, to give AT commands the Sichiray headset should be switched off
    if (c == 'M'){dataFlag = true;reduceFlag = true;atFlag = false;displayFlag = false;}  //measuring all data and print as semicolon seperated string, default mode and TWEDGE mode
               /*Signal fail;
                Delta;
                Theta;
                Low Alpha;
                High Alpha;
                Low Beta;
                High Beta;
                Low Gamma;
                Mid Gamma;
                Attention;
                Meditation */
     if (c == 'D'){dataFlag = true;reduceFlag = true;atFlag = false;displayFlag = true;}//print signal, attention and meditation with headers
      
 
  if (mySerial.available()) {
      //read as character, when dataFlag false, read as byte, when dataFlag true
      if (dataFlag == false)d = mySerial.read();
      else e = mySerial.read();
      // test if pause has occured between data sets and identify the begin of dataset:
      if (actualTime - readTime > 200){
        beginFlag = true;
        counter = 1;
        if (reduceFlag == false || displayFlag == true)Serial.println(); 
        //if (displayFlag == true)Serial.println("new data");
       }else beginFlag = false;
      
    if(atFlag == false){
      data[counter] = e;  
      counter++;  
      }
      
      readTime = millis();
      if  (reduceFlag == false){
        if (dataFlag == false)Serial.print(d);
        else Serial.print(e);
        if(atFlag == false)Serial.print (" "); 
        } else if (counter == 35){//when all 35 bytes have been arrived the data array can be analysed
         if (displayFlag == true){ Serial.print("signal\t");Serial.print(data[5]);}
         //digital filtering by about a factor of 3
          sig = sig*0.7 + data[5]*0.3; //signal fail value, sig = 0 is ideal, sig = 200 means no contact to skin
          Send = (String)sig + ";";
          // dechiffer spectral data
          for(i=10; i<32; i+=3){
            Wert[i] =  Wert[i]*0.7 + ((long)data[i]+ ((long)data[i-1])*256 + ((long)data[i-2])*256*256)*0.3;
            Send += (String)Wert[i] + ";";
           
            }
          
         if (displayFlag == true){Serial.print("\tattention\t");Serial.print(data[33]);}
         // add attention value to the string Send
         attention = attention*0.7 + data[33]*0.3;
          Send += (String)attention + ";";   // attention signal ranging from 0 to 100
          
         if (displayFlag == true){Serial.print("\tmeditation\t");Serial.print(data[35]);}
         // add meditation value to the string Send
           meditation = meditation*0.7 + data[35]*0.3;
           Send += (String)meditation;      // meditation signal ranging from 0 to 100

           elapse++;
            if (elapse > 2) {
            if (displayFlag == false)  Serial.println(Send);
              elapse = 0;
            }
         
           
          if (data[5] < 200){ 
            tone(13,  toene[(int)(data[33]/2)]);   
            analogWrite (6, (int)(data[35]*2.55));
            } else {
              noTone(13); //Turn Speaker off
              analogWrite (6, 0); //Turn LED off
              }
          
        }
     }
}
void autoconnect(void){
  mySerial.print('A');
  mySerial.print('T');
  mySerial.print('+');
  mySerial.print('I');
  mySerial.print('N');
  mySerial.print('Q');
  mySerial.print('\r');
  mySerial.print('\n');
  delay(5000);
  mySerial.print('A');
  mySerial.print('T');
  mySerial.print('+');
  mySerial.print('C');
  mySerial.print('O');
  mySerial.print('N');
  mySerial.print('N');
  mySerial.print('1');
  mySerial.print('\r');
  mySerial.print('\n');
  delay(2000);
}


