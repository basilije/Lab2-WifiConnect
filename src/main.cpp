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


void giveMeFive() {
  delay(5000);  // five second break
}

void printMainMenu() {  
  Serial.print("A – Display MAC address\nL - List available wifi networks\nC – Connect to a wifi network\nD – Disconnect from the network\nI – Display connection info\nM – Display the menu options\n\n");
  }

void printMacAddresses() {  
    WiFi.macAddress(mac);  // get your MAC address
    Serial.println(macAddressToString(mac));  // and print  your MAC address
}

void networkList() {
  num_ssid = WiFi.scanNetworks();   
  if (num_ssid != -1) {
    for (int this_net = 0; this_net < num_ssid; this_net++) {     
      Serial.print(this_net + 1);  // print the network number      
      Serial.println(". " + WiFi.SSID(this_net) + " [" + wifiAuthModeToString(WiFi.encryptionType(this_net)).c_str() + "]  (" + WiFi.RSSI(this_net)+" dBm)");  // print the ssid, encryption type and rssi for each network found:
    }
  }
  else
    Serial.println("Couldn't get a wifi connection");
}

void connect() {  
  networkList();
  int net_position_in_array = std::atoi(serialPrompt("\nChoose Network: ", 3).c_str()) - 1;
  String ssid = WiFi.SSID(net_position_in_array);
  String network_password = serialPrompt("Password: ", 32);
  const char* cch_ssid = ssid.c_str();
  const char* cch_net_pss = network_password.c_str();
  Serial.print("Connecting to "); Serial.print(cch_ssid); Serial.print("...\n\n");
  WiFi.begin(cch_ssid, cch_net_pss);
  giveMeFive();
  Serial.println(wifiStatusToString(WiFi.status()).c_str()); 
}

void disconnect() {
  Serial.print("Disonnecting...");
  WiFi.disconnect();
  giveMeFive();
  status = WiFi.status();
  Serial.println(wifiStatusToString(status).c_str());   
}

void connectionInfo() {
  Serial.print("Status:\t\t");  Serial.println(wifiStatusToString(WiFi.status()).c_str());
  Serial.print("Network:\t");  Serial.println(WiFi.SSID());
  Serial.print("IP Address:\t");  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask:\t");  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway:\t");  Serial.println(WiFi.gatewayIP());
} 


void setup(){
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(115200,  SERIAL_8N1); // initialize the serial port
  printMainMenu();
}

void loop() {
  Serial.println("–––––––––––––––––––––––––––––––––––––––––\n");
  delay(42); // is there the final answer, o mighty?

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
