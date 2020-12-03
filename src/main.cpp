/***********************************************************************************
* Project: Lab2 - Exercise #1 - Wifi connect
* Class: CIT324 - Networking for IoT
* Author: Vasilije Mehandzic
*
* File: main.cpp
* Description: Main file for Exercise #1
* Date: 12/3/2020
**********************************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include "serial-utils.h"
#include "wifi-utils.h"
// for disable brownout detector https://github.com/espressif/arduino-esp32/issues/863
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

int incoming_byte, num_ssid, key_index = 0;  // network key Index number
byte mac[6];
wl_status_t status = WL_IDLE_STATUS;  // the Wifi radio's status

/***********************************************************************************
* Purpose: Print the main menu content.
* No arguments, no returns
**********************************************************************************/
void printMainMenu() {  
  Serial.print("A – Display MAC address\nL - List available wifi networks\nC – Connect to a wifi network\nD – Disconnect from the network\nI – Display connection info\nM – Display the menu options\n\n");
  }

/***********************************************************************************
* Purpose: Print on the serial port the mac address in use.
* No arguments, no returns 
**********************************************************************************/
void printMacAddresses() {  
    WiFi.macAddress(mac);  // get your MAC address
    Serial.println(macAddressToString(mac));  // and print  your MAC address
}

/***********************************************************************************
* Purpose: Scan and detailed serial port print of the Network APs found
* No arguments, no returns
**********************************************************************************/
void networkList() {
  num_ssid = WiFi.scanNetworks();   
  if (num_ssid > -1) {
    for (int this_net = 0; this_net < num_ssid; this_net++) {     
      Serial.print(this_net + 1);  // print the network number      
      Serial.println(". " + WiFi.SSID(this_net) + " [" + wifiAuthModeToString(WiFi.encryptionType(this_net)).c_str() + "]  (" + WiFi.RSSI(this_net)+" dBm)");  // print the ssid, encryption type and rssi for each network found:
    }
  }
  else
    Serial.println("Couldn't get a wifi connection");
}

/***********************************************************************************
* Purpose: Connect to the chosen network from the list 
* No arguments, no returns
**********************************************************************************/
void connect() {  
  String ssid = WiFi.SSID(std::atoi(serialPrompt("\nChoose Network: ", 3).c_str()) - 1);
  String network_password = serialPrompt("Password: ", 42);  // that's it
  const char* cch_ssid = ssid.c_str();
  const char* cch_net_pss = network_password.c_str();
  Serial.print("Connecting to "); Serial.print(cch_ssid); Serial.print("...\n\n");
  WiFi.begin(cch_ssid, cch_net_pss);
  delay(2000);
  Serial.println(wifiStatusToString(WiFi.status()).c_str()); 
}

/***********************************************************************************
* Purpose: Disconnect WiFi and print the current status
* No arguments, no returns
**********************************************************************************/
void disconnect() {
  Serial.print("Disonnecting...");
  WiFi.disconnect();
  delay(2000);
  status = WiFi.status();
  Serial.println(wifiStatusToString(status).c_str());   
}

/***********************************************************************************
* Purpose: Print the connection info
* No arguments, no returns
**********************************************************************************/
void connectionInfo() {
  Serial.print("Status:\t\t");  Serial.println(wifiStatusToString(WiFi.status()).c_str());
  Serial.print("Network:\t");  Serial.println(WiFi.SSID());
  Serial.print("IP Address:\t");  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask:\t");  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway:\t");  Serial.println(WiFi.gatewayIP());
} 

/***********************************************************************************
* Purpose: Setup the port, detector and connect to the WhiskeyBug
* No arguments, no returns
**********************************************************************************/
void setup(){
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(115200,  SERIAL_8N1); // initialize the serial port
  printMainMenu();
}

void loop() {
  Serial.println("–––––––––––––––––––––––––––––––––––––––––\n");
  delay(1000); 

  incoming_byte = int(*serialPrompt("Choice: ", 1).c_str()); // read the key, convert to const char* and back to ascii int
  Serial.println("");
  
  if (incoming_byte > 96)  // to_upper string int function, based on ascii table
    incoming_byte -= 32; 

  switch (incoming_byte)  // to_upper calculation and next switch are based on http://www.asciitable.com/
  {        
    case 65:  // "A"=65; "a"=97
      printMacAddresses();
      break;
    
    case 76:  // "L"=76; "l"=108
      networkList();      
      break;
    
    case 67:  // "C"=67; "c"=99
      networkList();
      connect();
      break;
    
    case 68:  // "D"=68; "d"=100
      disconnect();
      break;
    
    case 73:  // "I"=73; "i"=105
      connectionInfo();
      break;
    
    case 77:  // "M"=77; "m"=109
      printMainMenu();
      break;                                 
  } 
}

