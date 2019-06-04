/*******************************************************************
    An example of bot that receives commands and turns on and off
    an LED.
 *                                                                 *
    written by Giacarlo Bacchio (Gianbacchio on Github)
    adapted by Brian Lough
 *******************************************************************/
//#include <ESP8266WiFi.h>  //include this for ESP8266
#include <WiFi.h>  //include this for Adafruit HUZZAH32

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include <time.h>
#include <TimeLib.h>    //https://github.com/PaulStoffregen/Time
int timezone = -6;      //MST
int dst = 0;        //I don't think that the core supports Daylight Savings time.

// Initialize Wifi connection to the router
char ssid[] = "TELUS0183";     // your network SSID (name)
char password[] = "6z5g4hbdxi"; // your network key

// Initialize Telegram BOT
//Bot name: PJ Test Bot #1
//Bot username: pjt1_bot
//Token: 566851939:AAFqXYOzRVA-FSjt3CL6ajrpZ5Odrx9s8Y4
//bot api https://core.telegram.org/bots/api

//How to create commands in telegram
//after bot has been created
// /setcommands
// then enter commands like this. all in one chat. It seems you have to add all commands at once. I am not sure how to just add a new command to the list.
/*
  start - start
  ledon - Turn LED On
  ledoff - Turn LED Off
  status - Get LED status
  name - What is your name
  time - Get Time
*/

//Bots can't initiate conversations with users. A user must either add them to a group or send them a message first.
//People can use telegram.me/<bot_username> links or username search to find your bot.

//Privacy mode is enabled by default for all bots, except bots that were added to the group as admins (bot admins always receive all messages). It can be disabled, so that the bot receives all messages like an ordinary user.

#define BOTtoken "566851939:AAFqXYOzRVA-FSjt3CL6ajrpZ5Odrx9s8Y4"  // your Bot Token (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;

//const int ledPin = 2;   //ESP8266
const int ledPin = 13;    //Adafruit HUZZAH32

int ledStatus = 0;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (bot.messages[i].longitude != 0 || bot.messages[i].latitude != 0) {
      Serial.print("Long: ");
      Serial.println(String(bot.messages[i].longitude, 6));
      Serial.print("Lat: ");
      Serial.println(String(bot.messages[i].latitude, 6));

      String message = "Long: " + String(bot.messages[i].longitude, 6) + "\n";
      message += "Lat: " + String(bot.messages[i].latitude, 6) + "\n";
      bot.sendMessage(chat_id, message, "Markdown");
    }

//response keyboard.
   if (text == "/options") {
      String keyboardJson = "[[\"/ledon\", \"/ledoff\"],[\"/status\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson, true);
    }

//inline keyboard
 //     if (text == "/options") {
 //       String keyboardJson = "[[{ \"text\" : \"Go to Google\", \"url\" : \"https://www.google.com\" }],[{ \"text\" : \"Send\", \"callback_data\" : \"This was sent by inline\" }]]";
 //       bot.sendMessageWithInlineKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson);
 //     }
    

    if (text == "/ledon") {
      //digitalWrite(ledPin, LOW);   // turn the LED on (HIGH is the voltage level)   //esp8266
      digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)     //Heather32
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
    }

    if (text == "/ledoff") {
      ledStatus = 0;
      //digitalWrite(ledPin, HIGH);    // turn the LED off (LOW is the voltage level)  //esp8266
      digitalWrite(ledPin, LOW);    // turn the LED off (LOW is the voltage level)    //Heather32
      
      bot.sendMessage(chat_id, "Led is OFF", "");
    }


    if (text == "/name") {
      bot.sendMessage(chat_id, "I am pjt1_bot. What do you want master?", "");
    }


    if (text == "/time") {
      //  time_t is used to store the number of seconds since the epoch (normally 01/01/1970)
      time_t now = time(nullptr);
      bot.sendMessage(chat_id, ctime(&now), "");      //return the current time

      //The Arduino Time library returns a time_t to the now() function.  time_t t = now(); // store the current time in time variable t. I am not sure why this doesn't work.
      //bot.sendMessage(chat_id, String(minute(now)), "");    //return the current minute

    }



    if (text == "/status") {
      if (ledStatus) {
        bot.sendMessage(chat_id, "Status: Led is ON", "");
      } else {
        bot.sendMessage(chat_id, "Status: Led is OFF", "");
      }
    }

    if (text == "/start") {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is PJ Bot example.\n\n";
      welcome += "/ledon : to switch the Led ON\n";
      welcome += "/ledoff : to switch the Led OFF\n";
      welcome += "/time : Returns current Date and Time\n";
      welcome += "/name : Returns bot name\n";
      welcome += "/status : Returns current status of LED\n";

      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
 // digitalWrite(ledPin, HIGH); // initialize pin as off    //esp8266
  digitalWrite(ledPin, LOW); // initialize pin as off    //Adafruit HUZZAH32

  //https://arduino.stackexchange.com/questions/42922/get-hour-with-ctime-time-library-with-esp8266
  //https://github.com/esp8266/Arduino/issues/2505
  //https://github.com/esp8266/Arduino/issues/1679

  //this sets the core to periodically sync the time. I don't really know how often this happens
  configTime(timezone * 3600, dst, "pool.ntp.org", "time.nist.gov");

}

void loop() {


  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
