#include <Keypad.h>
#include <LiquidCrystal.h>
#include "WiFi.h"
#include <HTTPClient.h>

#define DEVICE_NAME "game_3"

// Wi-Fi Credentials
const char* ssid = "UCSD-GUEST";
const char* password = "";

// Apps Script URL for Sheet
String Web_App_URL = "https://script.google.com/macros/s/AKfycby6tQuxwJNh96bDOGEywhGtjb037TwChgm3f7RJI5lEb7PyBRkS998mO30AnnCaW5f9rg/exec";

// Define constants
const String CLEAR_LINE = "                ";
const int NUM_PASSWORDS = 5;
const int PASSWORD_LENGTH = 4;

// ESP32 compatible pin assignments
// LCD pins: RS, Enable, D4, D5, D6, D7
LiquidCrystal lcd(19, 23, 18, 17, 16, 15);

// Define the series of passwords
String input;
String passwords[NUM_PASSWORDS] = {"2458", "8524", "5248", "4285", "8542"};
int correctCount = 0;
int gameStatus = 0;

// Initialize the keypad with ESP32 compatible pins
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
// Using safe ESP32 GPIO pins
byte rowPins[ROWS] = {14, 27, 26, 25}; // Row pins
byte colPins[COLS] = {33, 32, 35};     // Column pins

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(115200);
  delay(1000); // Give ESP32 time to stabilize
  Serial.println("ESP32 starting...");
  
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Initializing...");
  
  connectToWiFi();
}

void loop() {
  // Feed watchdog
  yield();
  
  // Check if all passwords have been entered correctly
  if (correctCount == NUM_PASSWORDS) {
    delay(1000);
    puzzleSolved();
  }

  char key = keypad.getKey();

  if (key) {
    input += key;
    lcd.setCursor(0, 1);
    lcd.print(CLEAR_LINE);
    lcd.setCursor(0, 1);
    lcd.print(input);

    if (input.length() == PASSWORD_LENGTH) {
      delay(1000);
      lcd.clear();
      if (input == passwords[correctCount]) {
        correctCount++;
        lcd.print("Correct");
      } else {
        lcd.print("Incorrect");
      }
      delay(1000);
      input = "";
      displayPrompt();
    }
  }
}

void puzzleSolved() {
  lcd.clear();
  lcd.print("Congratulations,");
  lcd.setCursor(0, 1);
  lcd.print("Puzzle Solved!");
  delay(10000);
  correctCount = 0;
  displayPrompt();
  
  if (WiFi.status() == WL_CONNECTED) {
    sendDataToGoogleSheet();
    displayNextGame();
  } else {
    lcd.clear();
    lcd.print("Offline Mode");
    lcd.setCursor(0, 1);
    lcd.print("Game completed!");
    delay(3000);
  }
}

void displayPrompt() {
  lcd.clear();
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
  lcd.print("Correct: ");
  lcd.print(correctCount);
  lcd.print(" of 5");
}

void connectToWiFi() {
  Serial.println("WIFI mode : STA");
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int attempts = 0;
  const int maxAttempts = 20;
  
  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    lcd.clear();
    lcd.print("Connecting...");
    lcd.setCursor(0, 1);
    lcd.print("Attempt: ");
    lcd.print(attempts + 1);
    delay(500);
    Serial.print(".");
    attempts++;
    
    // Feed watchdog during WiFi connection
    yield();
  }

  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.print("WiFi connected");
    Serial.println("WiFi connected");
    delay(1000);
  } else {
    lcd.clear();
    lcd.print("WiFi failed");
    lcd.setCursor(0, 1);
    lcd.print("Continue offline");
    Serial.println("WiFi connection failed - continuing offline");
    delay(2000);
  }
  
  displayPrompt();
}

void sendDataToGoogleSheet() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected - skipping data send");
    lcd.clear();
    lcd.print("Offline Mode");
    delay(1000);
    return;
  }

  String Send_Data_URL = Web_App_URL + "?action=espwrite&game=" + String(DEVICE_NAME);

  Serial.println("Send data to Google Spreadsheet...");
  Serial.print("URL : ");
  Serial.println(Send_Data_URL);

  lcd.clear();
  lcd.print("Sending data...");
  
  HTTPClient http;
  http.begin(Send_Data_URL.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  Serial.print("HTTP Status Code : ");
  Serial.println(httpCode);

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Payload : " + payload);
  } else {
    Serial.println("Failed to send data");
  }
  
  http.end();
  lcd.clear();
  lcd.print("Data sent");
  delay(2000);
  lcd.clear();
}

void displayNextGame() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected - skipping next game fetch");
    lcd.clear();
    lcd.print("Offline Mode");
    lcd.setCursor(0, 1);
    lcd.print("Ready to restart");
    delay(3000);
    return;
  }

  String nextGameUrl = Web_App_URL + "?action=espread&game=" + String(DEVICE_NAME);

  Serial.println("Receiving data from Google Spreadsheet...");
  Serial.print("URL : ");
  Serial.println(nextGameUrl);

  lcd.clear();
  lcd.print("Loading data...");
  
  HTTPClient http;
  http.begin(nextGameUrl.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  Serial.print("HTTP Status Code : ");
  Serial.println(httpCode);

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Payload : " + payload);

    lcd.clear();
    lcd.print("Next Game:");
    lcd.setCursor(0, 1);
    lcd.print(payload);
  } else {
    Serial.println("Failed to receive data");
    lcd.clear();
    lcd.print("Data fetch failed");
  }
  
  http.end();
  delay(5000);
}