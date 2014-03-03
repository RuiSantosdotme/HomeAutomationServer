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
#include <RCSwitch.h>


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
RCSwitch mySwitch = RCSwitch();  //Socket switch
int flag_1=0;                  // flag for the socket #1
int flag_2=0;                  // flag for the socket #2

/*******************************************************************************************
 ****     feel free to add more outputs, inputs or servos in the void setup()            ***
 *******************************************************************************************/
void setup(){
  //starts the serial communication for debugging purpose
  Serial.begin(9600);
  
  //initialize the 433mhz transmitter
  // Transmitter is connected to Arduino Pin #10  
  mySwitch.enableTransmit(10);
  // Optional set pulse length.
  mySwitch.setPulseLength(504); 
  // Optional set protocol (default is 1, will work for most outlets)
  // mySwitch.setProtocol(2);
  // Optional set number of transmission repetitions.
  mySwitch.setRepeatTransmit(30); 
  //the socket starts off by default
  mySwitch.send(1332564, 24);
  delay(300);
  
  //starts the Ethernet Connection
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore
    // try to congifure using IP address instead of DHCP
    Ethernet.begin(mac, ip);
   }
  // give the Ethernet shield a second to initialize
  delay(5000);
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
  //wait 5 seconds before reconnecting again
  delay(5000); 
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
  //when the button "Turn On" from the "Button #1" is Pressed
  if(pageState=="1" && flag_1==0){       
    mySwitch.send(XXXXXXX, 24);
    flag_1=1;
  }
  //when the button "Turn Off" from the "Button #1" is Pressed
  else if(pageState=="2" && flag_1==1){            
    mySwitch.send(XXXXXXX, 24);
    flag_1=0;
  }
  //when the button "Turn On" from the "Button #2" is Pressed
  if(pageState=="3" && flag_2==0){       
    mySwitch.send(XXXXXXX, 24);
    flag_2=1;
  }
  //when the button "Turn Off" from the "Button #2" is Pressed
  else if(pageState=="4" && flag_2==1){            
    mySwitch.send(XXXXXXX, 24);
    flag_2=0;
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
 //if the client is not avaibable when the stop_connection reaches 10000
 //it will restart the ethernet connection
 if(stop_connection > 10000){
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
