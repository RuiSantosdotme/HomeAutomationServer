/*
This code was created by Rui Santos
In order to use this code you must create your free account at
http://homeautomationserver.com

Thanks to:
Nelson Adriano
David A. Mellis
Adam Meyer
Barragam

If you have any questions please feel free to visit our forum
http://randomnerdtutorials.com/forum

Created in January 2014
Future code modfications can be found at:
https://github.com/RuiSantosdotme/HomeAutomationServer

*/

//This project Only works with ARDUINO IDE 1.0+
#include <Ethernet.h>
#include <SPI.h>
#include <Servo.h> 

/*******************************************************************************************
 ********                             CONFIGURURATION                                    ***
 *******************************************************************************************/
 
//server[] = { 192,168,1,73 }; //ip Address of the server you will connect to or the URL you want to connect to
//Please don't change the URL
char server[] = "www.homeautomationserver.com";

/*******************************************************************************************
 ****               You must replace the XXXXXXXXXXXXXX with your unique user_key        ***
 ****      we provide that key in the members area at http://homeautomationserver.com/   ***
 ****            this is an user_key example:   do13snpx37hici1                         ****
 *******************************************************************************************/

//keep HTTP/1.1 at the end. this is telling it what type of file it is
//Replace the XXXXXXXXXXXXXX with your unique user_key
String location = "/f/XXXXXXXXXXXXXX.txt HTTP/1.1";


// if need to change the MAC address (Very Rare)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0,177);

EthernetClient client;       // defines our client
char lastState[32];          // string for incoming serial data
int lastStatePos = 0;        // index counter for lastState variable
boolean startRead = false;   // if it's true is reading the page
int stop_connection = 0;     // if the client is not available stops the connection
int light_1 = 3;             // "Light #1" connected to the Ditial Pin3 of the Arduino
int light_2 = 4;             // "Light #2" connected to the Ditial Pin4 of the Arduino
int light_3 = 5;             // "Light #3" connected to the Ditial Pin5 of the Arduino
int garage_door = 6;         // "Garage Door" connected to the Ditial Pin6 of the Arduino
int home_door = 7;           // "Home Door" connected to the Ditial Pin7 of the Arduino
Servo baseServo;             // create servo object to control a servo                
Servo topServo;              // create servo object to control a servo 
int bPos = 0;                // variable to store the baseServo position 
int tPos = 0;                // variable to store the topServo position 

/*******************************************************************************************
 ****     feel free to add more outputs, inputs or servos in the void setup()            ***
 *******************************************************************************************/
void setup(){
  //starts the serial communication for debugging purpose
  Serial.begin(9600);
  //initialize the lights as outputs
  pinMode(light_1, OUTPUT); 
  pinMode(light_2, OUTPUT); 
  pinMode(light_3, OUTPUT);  
  //initialize the doors as outputs 
  pinMode(garage_door, OUTPUT); 
  pinMode(home_door, OUTPUT);
  //webcam servos
  baseServo.attach(8);      // attaches the servo on pin 8 to the servo object 
  topServo.attach(9);       // attaches the servo on pin 9 to the servo object
  
 /******You need to edit all the servos positions to fit your webcam setup!******/ 
  //turns the Camera to the "Center" position
  bPos=78;
  baseServo.write(bPos);              
  delay(50);                        
  tPos = 80;
  topServo.write(tPos);    
  delay(50);     
  //starts the Ethernet Connection
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore
    // try to congifure using IP address instead of DHCP
    Ethernet.begin(mac, ip);
   }
  // give the Ethernet shield a second to initialize
  delay(1000);
}

/*******************************************************************************************
 *********      you don't need to change anything in the void loop()             ***********
 *******************************************************************************************/
void loop(){
  //reset the stop_connection counter
  stop_connection = 0;
  //connects to your homeautomationserver file and read the current state
  String pageState = connect_and_read(); 
  //updates the Arduino state
  updateState(pageState);
  //prints the state in the serial monitor
  Serial.print("Current state:  ");
  Serial.println(pageState); 
  //wait 3.5 seconds before reconnecting again
  delay(3500); 
}
/*******************************************************************************************
 ****             Change the updateState(String pageState) function                 ********
 *******************************************************************************************
 This function tells your arduino exactly what to do when you press one button  in your
 dashboard at homeautomationserver.com. So simply change those if conditions to do whatever 
 you want. You can control lights with relays, servos, transistors, LED's, buzzers, etc...
 pretty much anything you desire.
 the pageState value is indicated at your dashboard: homeautomationserver.com/dashboard 
 there's a table at the end of the page which tells you the values of each button when
 it's pressed
 *******************************************************************************************/

void updateState(String pageState){
  //when the button "Turn On" from the "Light #1" is Pressed
  if(pageState=="1"){       
    digitalWrite(light_1, HIGH);
  }
  //when the button "Turn Off" from the "Light #1" is Pressed
  else if(pageState=="2"){            
    digitalWrite(light_1, LOW);
  }
  //when the button "Turn On" from the "Light #2" is Pressed
  else if(pageState=="3"){            
    digitalWrite(light_2, HIGH);
  }
  //when the button "Turn Off" from the "Light #2" is Pressed
  else if(pageState=="4"){            
    digitalWrite(light_2, LOW);
  }
  //when the button "Turn On" from the "Light #3" is Pressed
  else if(pageState=="5"){            
    digitalWrite(light_3, HIGH);
  }
  //when the button "Turn Off" from the "Light #3" is Pressed
  else if(pageState=="6"){            
    digitalWrite(light_3, LOW);
  }
  //when the button "Turn On" from the "All lights" is Pressed
  else if(pageState=="7"){            
    digitalWrite(light_1, HIGH);
    digitalWrite(light_2, HIGH);
    digitalWrite(light_3, HIGH);
  }
  //when the button "Turn Off" from the "All lights" is Pressed
  else if(pageState=="8"){            
    digitalWrite(light_1, LOW);
    digitalWrite(light_2, LOW);
    digitalWrite(light_3, LOW);
  }
  //when the button "Open" from the "Garage Door" is Pressed
  else if(pageState=="9"){            
    digitalWrite(garage_door, HIGH);
  }
  //when the button "Close" from the "Garage Door" is Pressed
  else if(pageState=="10"){            
    digitalWrite(garage_door, LOW);
  }
  //when the button "Open" from the "Home Door" is Pressed
  else if(pageState=="11"){            
    digitalWrite(home_door, HIGH);
  }
  //when the button "Close" from the "Home Door" is Pressed
  else if(pageState=="12"){            
    digitalWrite(home_door, LOW);
  }
  
/******You need to edit all the servos positions to fit your webcam setup!******/
  //when the button "Camera Up" from the "Camera" is Pressed
  else if(pageState=="13" && bPos!=35){            
    for(bPos; bPos>35; bPos-=1){          // goes from 0 degrees to 180 degrees in steps of 1 degree                                     
      baseServo.write(bPos);              // tell servo to go to position in variable 'bPos' 
      delay(20);                          // waits 20ms for the servo to reach the position 
    } 
  }
  //when the button "Camera Down" from the "Camera" is Pressed
  else if(pageState=="14" && bPos!=120){     
    for(bPos; bPos < 120; bPos += 1){     // goes from 0 degrees to 180 degrees    in steps of 1 degree 
      baseServo.write(bPos);              // tell servo to go to position in variable 'bPos' 
      delay(20);                          // waits 20ms for the servo to reach the position 
    }      
  }
  //when the button "Camera left" from the "Camera" is Pressed
  else if(pageState=="15" && tPos!=20){  
     for(tPos; tPos>20; tPos-=1){         // goes from 0 degrees to 180 degrees on steps of 1 degree                                        
        topServo.write(tPos);             // tell servo to go to position in variable 'tPos' 
        delay(20);                        // waits 20ms for the servo to reach the position 
      }     
   
  }
  //when the button "Center" from the "Camera" is Pressed
  else if(pageState=="16"){   
    bPos=78;
    baseServo.write(bPos);              
    delay(50);                        
    tPos = 80;
    topServo.write(tPos);    
    delay(50);       
  }
  //when the button "Camera Right" from the "Camera" is Pressed
  else if(pageState=="17" && tPos!=140){            
     for(tPos; tPos < 140; tPos += 1){     // goes from 0 degrees to 180 degrees in steps of 1 degree                                        
        topServo.write(tPos);              // tell servo to go to position in variable 'tPos' 
        delay(20);                         // waits 20ms for the servo to reach the position 
     } 
  }

}
/*******************************************************************************************
 *******************************************************************************************
 ************************    Don't change the code below    ********************************
 *********     If you want to improve the code  feel free to do so             *************
 *  tell me what you improved at: https://github.com/RuiSantosdotme/HomeAutomationServer   *
 *******        or at our forum: http://randomnerdtutorials.com/forum                  *****
 *******************************************************************************************
 *******************************************************************************************/

//connect to the server and read the state
String connect_and_read(){
  //connecting to the server
  Serial.println("connecting...");

  //port 80 is typical of a www or http:// page
  if (client.connect(server, 80)) {
    Serial.println("connected");
    //the exact location you want to connect to
    client.print("GET ");
    client.println(location);
    //your host
    client.println("Host: www.homeautomationserver.com");
    client.println("Connection: close");
    client.println();
    //it's Connected - So let's read the state on our page
    return readState(); 
  }
  //if the client can't connect to the server
  else{
    //restarts the ethernet
    restart();
    //tells the user that failed the connection
    return "connection failed"; 
  }
}
//reads the state of the button your pressed on the website
String readState(){
  //resets the position counter
  lastStatePos = 0;
  //clear lastState memory
  memset( &lastState, 0, 32 ); 
  
  while(true){
    //checks if the client is available
    if (client.available()) {
      //stores what is reading in the char c
      char c = client.read();
      //'<' is our begining character, our state is between '<>' for example: <1>
      if (c == '<' ) { 
        //Now is Ready to start reading the page
        startRead = true;  
      }
      //after reading the '<'
      else if(startRead){
          //'>' is our ending character
          if(c != '>'){ 
          lastState[lastStatePos] = c;
          lastStatePos ++;
          }
          else{
            //got what we need here! We can disconnect now
            startRead = false;
            client.stop();
            client.flush();
            Serial.println("disconnecting.");    
            //returns the last button pressed    
            return lastState;
          }
        }
    }
    
 stop_connection ++;
 //if the client is not avaibable when the stop_connection reaches 8000
 //it will restart the ethernet connection
 if(stop_connection > 8000){
   //stops the client
   client.stop();
   delay(50);
   Serial.println("The Client is not available.");
   delay(50);
   //restarts the ethernet connection
   restart();
   delay(100);
   return "Error during connection, stop_connection was activated";
 }
  }//where the while(true) ends

}

//restarts the ethernet connection
void restart(){
  Serial.println("restarting...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
}
