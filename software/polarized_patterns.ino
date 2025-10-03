// ========== LIBRARIES ==========
#include "WiFi.h"
#include <Keypad.h>
#include <HTTPClient.h>
#include <LiquidCrystal.h>

// ========== Google Sheet Communications ==========
#define DEVICE_NAME "game_3"
#define NUM_GROUPS 5 // change based on the max number of groups in the session

String groupCodes[NUM_GROUPS] = {"738", "291", "465", "823", "679"};
String currentGroupId = ""; // stores ID of the currently logged in group
bool groupLoggedIn = false;

// Wi-Fi Credentials
const char* ssid = "UCSD-GUEST"; // Wi-Fi SSID
const char* password = ""; // Wi-Fi Password (empty if none)
unsigned long lastWiFiCheckTime = 0;
const unsigned long wifiCheckInterval = 10000; // check Wi-Fi connection every 10 seconds

// Apps Script URL for Sheet
String Web_App_URL = "https://script.google.com/macros/s/AKfycbylYj14j-Y-WTBLZ2l4LxsU1RmxNhvZmAZLOcgfZTPV1ZZnaDTxIpyOGvEXcoTHCaVheA/exec";

bool gameStatus = 0; // game status monitor

// LCD Pin Definitions for ESP32
#define RS 23
#define  E 22
#define D4 18
#define D5  5
#define D6 17
#define D7 16

// ===== LCD initialization =====
LiquidCrystal lcd(RS, E, D4, D5, D6, D7); // I2C LCD is not used for this game
const String CLEAR_LINE = "                "; // flushes a line on the LCD
String currentAttempt = ""; // stores current passcode attempt
int correctCount = 0; // cound of correctly inputted passcodes
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// ====== 3 CODE VERSION ======
const int NUM_PASSCODES = 3;
const String passcodes[NUM_PASSCODES] = {"2458", "8542", "4528"};

// ====== 5 CODE VERSION ======
// const int NUM_PASSCODES = 5;
// const String passcodes[NUM_PASSCODES] = {"2458", "2584", "8542", "4528", "2548"};

// ====== 10 CODE VERSION ======
// const int NUM_PASSCODES = 10;
// const String passcodes[NUM_PASSCODES] = {"2458", "8524", "2584", "8425", "8542", "4852", "2854", "4528", "5248", "4285"};

// ===== Keypad initialization =====
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {32, 33, 25, 26};
byte colPins[COLS] = {27, 14, 12};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  connectToWiFi();
  lcd.begin(LCD_COLS, LCD_ROWS);
}

void loop() {
  // background Wi-Fi check
  if (millis() - lastWiFiCheckTime >= wifiCheckInterval) {
    lastWiFiCheckTime = millis();

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected. Reconnecting...");
      connectToWiFi();
    }
  }

  // wait for a group to log in before the game can be played
  if (!groupLoggedIn) {
    handleLogin();
    sendGameStart();
    displayPrompt();
    return;
  }

  // check if all passcodes have been entered correctly
  if (correctCount == NUM_PASSCODES) {
    puzzleSolved();
    displayNextGame();
    handleLogout();
    return;
  }

  char key = keypad.getKey();

  if (key) {
    if (key == '#') {
      lcd.clear();
      if (currentAttempt == passcodes[correctCount]) {
        correctCount++;
        lcd.print("Correct passcode");
        lcd.setCursor(0, 1);
        lcd.print("Correct: ");
        lcd.print(correctCount);
        lcd.print("/");
        lcd.print(NUM_PASSCODES);
      } else {
        lcd.print("Incorrect please");
        lcd.setCursor(0, 1);
        lcd.print("try again");
      }
      delay(2000); // give players a chance to read feedback

      /**
      // Uncomment this code when using the 5-code or 10-code version
      if (correctCount > 0) {
        lcd.setCursor(0,1);
        lcd.print("Prev. Code: ");
        lcd.print(passcodes[correctCount-1]);
        delay(3000); // Display previous code for 4 seconds
      }
      */

      currentAttempt = "";
    } else if (key == '*' && !currentAttempt.isEmpty()) {
      currentAttempt.remove(currentAttempt.length()-1); // delete one digit from input
    } else {
      currentAttempt += key; // append keypress to current attempt
    }
    displayPrompt();
    lcd.setCursor(0, 1);
    lcd.print(CLEAR_LINE);
    lcd.setCursor(0, 1);
    lcd.print(currentAttempt);
  }
}

/**
 * Displays a congratulations message when the puzzle is solved.
 * Resets the game for the next group of players.
 */
void puzzleSolved() {
  // reset the game for the next group
  correctCount = 0;
  
  // display congrats message for 5 seconds
  lcd.clear();
  lcd.print("Puzzle Solved!");
  delay(5000); // display puzzle solved message 5 seconds
}

/**
 * Prompt players to either enter their group ID or a new passcode attempt
 * depending on whether a group is currently logged in.
 */
void displayPrompt() {
  lcd.clear();
  if (groupLoggedIn) {
    lcd.print("Enter Passcode:");
  } else {
    lcd.print("Enter Group ID:");
  }
}

// !!! NOTICE: below, I used showMessage() and other function to help with displaying via LCD, if you want to keep it original, just use native LCD codes to display messages

// ========== Google Sheet Communication FUNCTIONS ==========
void connectToWiFi() {

  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);
    }
  }
}

// Send game status to Google Sheets
void sendGameStart() {
  if (WiFi.status() == WL_CONNECTED && !currentGroupId.isEmpty()) {
    String url = Web_App_URL + "?action=start" + "&game=" + DEVICE_NAME + 
                  "&group=group_" + currentGroupId;

    HTTPClient http;
    http.begin(url.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();

    if (httpCode < 0) {
      showMessage("Request failed!", 0, true, 0);
      showMessage("See a staff", 1, false, 2000);
    }
    http.end();
  } else {
    showMessage("WiFi not connected!", 0, true, 0);
  }
}

// receive data and display next game
void displayNextGame() {
  if (WiFi.status() == WL_CONNECTED) {
    // construct the request URL
    String nextGameUrl = Web_App_URL + "?action=done" + "&game=" + DEVICE_NAME +
                          "&group=group_" + currentGroupId;

    showMessage("Loading data...", 0, true, 0);
    
    HTTPClient http;
    http.setConnectTimeout(5000);  // 5 seconds
    http.begin(nextGameUrl.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();

      // display next game
      showMessage("Next Game: ", 0, true, 0);
      showMessage(payload, 1, false, 5000);

      showMessage("Press the button", 0, true, 0);
      showMessage("to see magic ;)", 1, false, 0);
    } else {
      showMessage("Data fetch failed!", 0, true, 1000);
      showMessage("Please contact", 0, true, 0);
      showMessage("a staff!", 1, false, 5000);
      
      showMessage("Press the button", 0, true, 0);
      showMessage("to see magic ;)", 1, false, 0);
    }
    
    http.end();  // close the HTTP connection

  } else {
    // Serial.println("WiFi not connected");
    showMessage("WiFi not connected!", 0, true, 0);
  }
}

// log in group using their group ID
void handleLogin() {
  displayPrompt();
  while (!groupLoggedIn) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        lcd.clear();
        // check if the entered group ID is valid
        for (int i = 0; i < NUM_GROUPS; i++) {
          if (currentGroupId.equals(groupCodes[i])) {
            groupLoggedIn = true;
            lcd.print("Group logged in");
            lcd.setCursor(0, 1);
            lcd.print("with ID: ");
            lcd.print(currentGroupId);
            delay(2000);
            return;
          }
        }
        lcd.print("Invalid group ID");
        lcd.setCursor(0, 1);
        lcd.print("Please try again");
        delay(2000);
        currentGroupId = "";
      } else if (key == '*' && !currentGroupId.isEmpty()) {
        currentGroupId.remove(currentGroupId.length()-1);
      } else {
        currentGroupId += key;
      }
      displayPrompt();
      lcd.setCursor(0, 1);
      lcd.print(CLEAR_LINE);
      lcd.setCursor(0, 1);
      lcd.print(currentGroupId);
    }
  }
}

// log out group from the game
void handleLogout() {
  currentGroupId = "";
  groupLoggedIn = false;
}

// ========== LCD FUNCTIONS ==========
// display helper
void showMessage(const String &msg, int row, bool clearFirst, unsigned long waitMs) {
  if (clearFirst) lcd.clear();
  lcd.setCursor(0, row);
  if (msg.length() <= LCD_COLS) {
    lcd.print(msg);
  } else {
    scrollMessage(msg, row, 200);
  }
  if (waitMs) delay(waitMs);
}

// scroll long messages
void scrollMessage(const String &msg, int row, unsigned long delayMs) {
  int len = msg.length();
  int total = len + LCD_COLS;
  for (int offset = 0; offset <= total; offset++) {
    lcd.setCursor(0, row);
    for (int i = 0; i < LCD_COLS; i++) {
      int idx = offset + i - LCD_COLS;
      if (idx >= 0 && idx < len) lcd.print(msg[idx]); else lcd.print(' ');
    }
    delay(delayMs);
  }
}
