# Freezer_Duck
-80 sensor that sends email warnings when temperature rises.

<img src="https://github.com/MyersResearchGroup/Freezer_Duck/blob/main/images/freezerducklogo1.jpeg#gh-light-mode-only" alt="PUDU logo" width="250"/>
<img src="https://github.com/MyersResearchGroup/Freezer_Duck/blob/main/images/freezerducklogo1.jpeg#gh-dark-mode-only" alt="PUDU night logo" width="250"/>

## Steps for set-up##
1. Acquire parts and wire up microcontroller and sensor properly
2. Install arduino (if not already on device) and packages needed to run script
3. Adjust Wifi login information to local Wifi using WIFI_SSID and WIFI_PASSWORD
4. Adjust SMTP email host according to email server email is using by adjusting SMTP_HOST and SMTP_PORT
5. Adjust sender email to desired email using AUTHOR_EMAIL and AUTHOR_PASSWORD
6. adjust recipient email by adding emails to the list in *Recipient_email[]={"email1","email2"}

## Parts needed ##
 - Arduino nano 33 IOT or ESP32 as microcontroller
 - Pmod TC1

##  Pinout ##
  Pmod TC1 <----------> Arduino nano 33 IOT
  
  VCC     to          3v3
  
  GND     to          GND
  
  SCK     to          13
  
  MISO    to          12
  
  CS      to          10
  
Associated pins for ESP32 or wifi enabled microcontroller can be found when looking at associated pinout guide.

## Libraries to be added ##
Board package (either ESP dev board of the arduino SAMD boards)
libraries from library manager:
- ESP_mail_client (for configuring email format)
- WiFiNina (if using arduino nano iot 33)
- Adafruit_MAX31855 (for thermosensor)
- ArduinoHttpClient (for interfacing with http)

## Changes to base code ##
 - Wifi edits
   - WIFI_SSID (change to network name of wifi used)
   - WIFI_PASSWORD (change to network password leave "" if no password needed)
 - Email port
   - if not using an outlook email change SMTP_HOST and SMTP_PORT to appropriate host/post (information about this can be found by Rui Santos' project information)
 - Email sender
   - create specific email account for device or use existing email
   - change AUTHOR_EMAIL to desired sender and AUTHOR_PASSWORD to the sender email's password
 - Email recipents
   - in *Recipient_email[]={"email1","email2"} change the emails to the desired email recievers

## Limitations on current system ##
This system is not the most exact system for below -70°C due to limitations of the thermocoupling wire. This means that for the system it is more of a warning system for extreme drops in tempurature that are dangerous towards preservation of biological materials. Due to this limitation drops above -60°C to -65°C should be used as a marker that something is wrong in this system. 

## Issues with current code ##
Due to believed issues with the wifi system at CU Boulder, the http/https system that exists with interfacing with the email server has authetication issues. This could also be caused by the change in authetification used by outlook to check SMTP communication. Provided it is a wifi issue, consulting with OIT to discuss firewall work arounds would be beneficial or having a in-lab wifi router to avoid communication issues. 
