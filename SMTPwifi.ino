/*
  Rui Santos
  Complete project details at:
   - ESP32: https://RandomNerdTutorials.com/esp32-send-email-smtp-server-arduino-ide/
   - ESP8266: https://RandomNerdTutorials.com/esp8266-nodemcu-send-email-smtp-server-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  Example adapted from: https://github.com/mobizt/ESP-Mail-Client

  Wiring 2ith use of Arduino nano 33 iot
  Module <----------> Arduino
  VCC     to          3v3
  GND     to          GND
  SCK     to          13
  MISO    to          12
  CS      to          10

  This program works properly if the email host can be reached, unfortunately the email host is no longer available to be SMTPed through. This could be due to WiFi connection issues on campus or outlook authetication 
*/

#include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <ESP_Mail_Client.h>
#include <SPI.h>
#include <Adafruit_MAX31855.h>
#include <ArduinoHttpClient.h>

#define WIFI_SSID "UCB Guest"
#define WIFI_PASSWORD ""
#define CS 10 //chip select pin
Adafruit_MAX31855 thermocouple(CS);

bool alarm_triggered= 0;
bool alarm_lock =0;
int temp = -80;
int numberPeople=1;
unsigned long StartTime=0;

/** The smtp host name e.g. smtp.gmail.com for GMail or smtp.office365.com for Outlook or smtp.mail.yahoo.com */
//attempted to run through sendgrid, but ultimately failed due to https vs. http issues when syncing

#define SMTP_HOST "api.sendgrid.com/v3/mail/send"
#define SMTP_PORT 587

/* The sign in credentials */
#define AUTHOR_EMAIL "YXBpa2V5"
#define AUTHOR_PASSWORD "NHM5LXVVN3hUODI0RzE4NGYtN0FIUS4xNVJxZ3Z2REJLZ09McWtsalQ3MDJQQUtaZkp0UEdtV2taalB2bHFORC1v"

/* Recipient's email*/
//# define RECIPIENT_EMAIL "parker.ackerknecht@colorado.edu"
char *RECIPIENT_EMAIL[] ={"freezerDuck@outlook.com"};
// char *RECIPIENT_EMAIL[] ={"freezerDuck@outlook.com","chris.myers@colorado.edu","Gonzalo.VidalPena@colorado.edu"};

/* Declare the global used SMTPSession object for SMTP transport */
SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

void setup(){
  Serial.begin(115200);
  thermocouple.begin();
  Serial.println(SMTP_HOST);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(30000);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
    
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /*  Set the network reconnection option */
  MailClient.networkReconnect(true);

  /** Enable the debug via Serial port
   * 0 for no debugging
   * 1 for basic level debugging
   *
   * Debug port can be changed via ESP_MAIL_DEFAULT_DEBUG_PORT in ESP_Mail_FS.h
   */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the Session_Config for user defined session credentials */
  Session_Config config;

  /* Set the session config */
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email =AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.login.user_domain = "";

  /*
  Set the NTP config time
  For times east of the Prime Meridian use 0-12
  For times west of the Prime Meridian add 12 to the offset.
  Ex. American/Denver GMT would be -6. 6 + 12 = 18
  See https://en.wikipedia.org/wiki/Time_zone for a list of the GMT/UTC timezone offsets
  */

  
  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = -6;
  config.time.day_light_offset = 0;
  

  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = F("FreezerDuck");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = F("test");
  message.addRecipient(F("parker"), RECIPIENT_EMAIL[0]);
    
  /*Send HTML message*/
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>FreezerDuck is up and ready</h1></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

   
  //Send raw text message
  /*
  String textMsg = "Hello World! - Sent from board";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay; */


  /* Connect to the server */
  if (!smtp.connect(&config)){
    ESP_MAIL_PRINTF("Connection error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    return;
  }

  if (!smtp.isLoggedIn()){
    Serial.println("\nNot yet logged in.");
  }
  else{
    if (smtp.isAuthenticated())
      Serial.println("\nSuccessfully logged in.");
    else
      Serial.println("\nConnected with no Auth.");
  }

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    ESP_MAIL_PRINTF("Error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
 
}

void loop(){
  readTempurature();
  if (((millis()-StartTime)>300000)&&(alarm_triggered==1)&&((millis()-StartTime)!=millis())&&(alarm_lock == 0) ){
    
    error();
    }
    delay(3000);

    if(((millis()-StartTime)>(10800000))&&(alarm_lock=1)){
      //alarm_lock=0;
      }
}

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    // ESP_MAIL_PRINTF used in the examples is for format printing via debug Serial port
    // that works for all supported Arduino platform SDKs e.g. AVR, SAMD, ESP32 and ESP8266.
    // In ESP8266 and ESP32, you can use Serial.printf directly.

    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);

      // In case, ESP32, ESP8266 and SAMD device, the timestamp get from result.timestamp should be valid if
      // your device time was synched with NTP server.
      // Other devices may show invalid timestamp as the device time was not set i.e. it will show Jan 1, 1970.
      // You can call smtp.setSystemTime(xxx) to set device time manually. Where xxx is timestamp (seconds since Jan 1, 1970)
      
      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
      
      if(alarm_triggered==1 && status.completedCount() > 1){
        Serial.println("alarm on and alarm sent successfully");
        }
    }
    Serial.println("----------------\n");

    // You need to clear sending result as the memory usage will grow up.
    smtp.sendingResult.clear();
  }
}

void error(){
  if(WiFi.status() != WL_CONNECTED){
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(30000);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("reconnect");
  }
  }
/*  Set the network reconnection option */
  MailClient.networkReconnect(true);
alarm_lock=1;
  /** Enable the debug via Serial port
   * 0 for no debugging
   * 1 for basic level debugging
   *
   * Debug port can be changed via ESP_MAIL_DEFAULT_DEBUG_PORT in ESP_Mail_FS.h
   */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the Session_Config for user defined session credentials */
  Session_Config config;

  /* Set the session config */
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.login.user_domain = "";
  
for(int x=0; x<numberPeople;x++){
  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = F("FreezerDuck");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = F("[Urgent] Freezer");
  message.addRecipient(F("Lab Monitors"), RECIPIENT_EMAIL[x]);
    
  /*Send HTML message*/
  String htmlMsg = "<div style=\"color:#2f4468;\"><p1>FreezerDuck is up and readyFreezer has thrown a tempurature error. Current reading is at</p1></div>";
  String variable = String(temp);
  
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.html.content = variable.c_str();
  message.html.content = variable.c_str();
  message.html.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  

  /* Connect to the server */
  if (!smtp.connect(&config)){
    ESP_MAIL_PRINTF("Connection error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    return;
  }

  if (!smtp.isLoggedIn()){
    Serial.println("\nNot yet logged in.");
  }
  else{
    if (smtp.isAuthenticated())
      Serial.println("\nSuccessfully logged in.");
    else
      Serial.println("\nConnected with no Auth.");
  }

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    ESP_MAIL_PRINTF("Error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
}
  }

void readTempurature(){
  temp= thermocouple.readCelsius();
  String variable = String(temp);
  Serial.print("string:");
  Serial.println(variable);
  Serial.print("int:");
  Serial.println(temp);
 if(temp<-57){
  StartTime=0;
  alarm_triggered=0;
  //alarm_lock=0;
  }
  if(temp>(-57)){ 
    if(alarm_triggered == 0){
      StartTime=millis();
      }
      alarm_triggered=1;
    }
  }
