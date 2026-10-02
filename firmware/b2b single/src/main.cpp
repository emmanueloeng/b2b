// YWROBOT
// Compatible with the Arduino IDE 1.0
// Library version:1.1
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Adafruit_Keypad.h"
#include <BluetoothSerial.h>
#include <FS.h>
#include <SPIFFS.h>
#include <set>       
#include <vector> 

#define LOCK1 17
#define BUZZER 23

char codes[50][7]; // 50 codes, each up to 6 chars + null terminator
char usedCodes[50][7];

const char *defaultCodes[50] = {
    "A4C7B2", "196C5A", "A7C43B", "5823B1", "C91AB6", "7A6C98", "1C5B7A", "3B124C", "75919A", "C1B8AC",
    "A4C58B", "839C2B", "9A8C7B", "45B1C7", "2C8B13", "C51A97", "62B8C1", "A94C1B", "718C32", "7CB7A9",
    "A9B883", "251C98", "C1B4A7", "58C3A1", "3C92B7", "7B4186", "C82AB3", "1A5C34", "C3B27A", "6B8521",
    "4CA219", "A1B97C", "7C3982", "59ACB7", "C8B4A1", "7B2A93", "8B4C1A", "3C1A97", "62B9C3", "C8A471",
    "5B9C13", "7C2A98", "C1B52A", "A7C9B3", "34B8C1", "7C7A95", "1B4C96", "8A6C2B", "3C75A9", "B1C4A7"};

const byte ROWS = 4; // rows
const byte COLS = 4; // columns
// define the symbols on the buttons of the keypads
char mkeys[ROWS][COLS] = {
    {'1', '4', '7', '*'},
    {'2', '5', '8', '0'},
    {'3', '6', '9', '#'},
    {'A', 'B', 'C', 'D'}};

byte colPins[ROWS] = {36, 39, 34, 35}; // connect to the row pinouts of the keypad
byte rowPins[COLS] = {32, 33, 25, 26}; // connect to the column pinouts of the keypad

// Component Initializations
Adafruit_Keypad keys = Adafruit_Keypad(makeKeymap(mkeys), colPins, rowPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 20, 4);
BluetoothSerial btclassic;

void centerString(String message, int row);
// void loadArray();
void printMessage(String msg);
String getCode(int length, int row);
void playTone(int pin, int frequency, int duration);
void failTone();
void successTone();
void clearDisplay();
bool codeExists(const String &code, char array[][7], int count);
void codeSuccess();
void launchSettings();
void addCode();
void cleanCodes();
bool appendCode(const String &code);
bool codeExistsSPIFF(const String &code);
void saveAllCodes(const char *codes[], int count);
void listCodes();
bool isInFile(const char *path, const String &code);
void appendToFile(const char *path, const String &code);
bool listCodesBt(const char *path);

void setup()
{
  Serial.begin(115200);
  SPIFFS.begin(true);
  cleanCodes();
  delay(1000);
  // saveAllCodes(defaultCodes, 50);
  // listCodes();
  lcd.init();
  lcd.backlight();
  keys.begin();
  btclassic.begin("MyBox");
  pinMode(BUZZER, OUTPUT);
  pinMode(LOCK1, OUTPUT);
  centerString("MyBox", 1);
  digitalWrite(BUZZER, 1);
  delay(1500);
  digitalWrite(BUZZER, 0);
  lcd.clear();
}

void loop()
{
  lcd.clear();
  centerString("Enter Lock Code", 0);
  String code = getCode(6, 1);

  if (code.length() < 6)
  {
    clearDisplay();
    printMessage("Incomplete Input!");
    failTone();
    delay(1000);
  }

  // check if already used
  else if (isInFile("/usedCodes.txt", code))
  {
    lcd.clear();
    centerString("Code Used", 0);
    centerString("Already!", 1);
    failTone();
    delay(1000);
    return;
  }
  // check if valid
  else if (code.equals("CBA456") || isInFile("/codes.txt", code))
  {
    if (!code.equals("CBA456"))
    {
      appendToFile("/usedCodes.txt", code);
    }
    codeSuccess();
    return;
  }

  // admin code
  else if (code.equals("246BAA"))
  {
    lcd.clear();
    centerString("Launching Settings", 1);
    successTone();
    delay(1000);
    launchSettings();
    return;
  }
  else
  {
    // wrong code
    printMessage("Wrong Code!");
    failTone();
    delay(1000);
  }
}

void printMessage(String msg)
{
  Serial.print(msg);
  btclassic.println(msg);
  lcd.clear();
  centerString(msg, 1);
}

void centerString(String message, int row)
{
  int messageLength = message.length();
  int spaces = (20 - messageLength) / 2;
  lcd.setCursor(0, row);
  lcd.print(" ");
  lcd.setCursor(spaces, row);
  lcd.print(message);
}

String getCode(int length, int row)
{
  String code = "";

  while (true)
  {
    keys.tick();

    while (keys.available())
    {
      keypadEvent e = keys.read();
      char key = (char)e.bit.KEY;

      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        playTone(BUZZER, 1000, 100);

        if (key == '#')
        {
          // Delete last character
          if (!code.isEmpty())
          {
            code.remove(code.length() - 1);
          }
        }
        else if (key == 'D')
        {
          // Enter key
          return code;
        }
        else if (code.length() < length)
        {
          // Add key to code
          code += key;
        }

        // Update LCD with current code
        lcd.setCursor(1, row);
        lcd.print("                "); // clear line
        lcd.setCursor(1, row);
        lcd.print(code);
      }
    }

    if (code.length() >= length)
    {
      return code;
    }
  }
}

void codeSuccess()
{
  successTone();
  printMessage("Opening Box...");
  digitalWrite(LOCK1, 1);
  delay(1000);
  digitalWrite(LOCK1, 0);
}

void playTone(int pin, int frequency, int duration)
{
  tone(pin, frequency, duration);
  delay(duration);
  noTone(pin);
}

void successTone()
{
  tone(BUZZER, 1500, 200);
  delay(200);
  noTone(BUZZER);
}

void failTone()
{
  tone(BUZZER, 500, 500);
  delay(500);
  noTone(BUZZER);
}

void clearDisplay()
{
  lcd.clear();
  lcd.setCursor(0, 0);
}

bool codeExists(const String &code, char array[][7], int count)
{
  for (int i = 0; i < count; ++i)
  {
    if (String(array[i]) == "------")
      continue; // skip placeholder
    if (code.equals(array[i]))
    {
      return true;
    }
  }
  return false;
}

void addCode()
{
  lcd.clear();
  centerString("Add Code:", 0);
  String code = getCode(6, 1);

  lcd.clear();
  {
    String msg = "Add " + code + " ?";
    centerString(msg, 0);
  }

  centerString("Y => 1, N => 2", 1);
  String confirm = getCode(1, 2);
  if (confirm.equals("1"))
  {
    lcd.clear();
    {
      appendToFile("/codes.txt", code);
      String msg = code + " added";
      centerString(msg, 0);
      centerString("to device!", 1);
      successTone();
      delay(1000);
    }
  }
  else
  {
    lcd.clear();
    centerString("Cancelled!", 0);
    failTone();
    delay(1000);
  }
}

bool appendCode(const String &code)
{
  if (codeExistsSPIFF(code))
  {
    Serial.println("⚠️ Code already exists.");
    return false;
  }

  File file = SPIFFS.open("/codes.txt", FILE_APPEND);
  if (!file)
  {
    Serial.println("❌ Failed to open file for appending");
    return false;
  }

  file.println(code);
  file.close();
  Serial.printf("✅ Code [%s] saved.\n", code.c_str());
  return true;
}

bool codeExistsSPIFF(const String &code)
{
  File file = SPIFFS.open("/codes.txt", FILE_READ);
  if (!file)
  {
    Serial.println("❌ Failed to open file for reading");
    return false;
  }

  while (file.available())
  {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line == code)
    {
      file.close();
      return true;
    }
  }

  file.close();
  return false;
}

void launchSettings()
{
  clearDisplay();
  centerString("SETTINGS", 0);
  lcd.setCursor(0, 1);
  lcd.print("1. Add Code");
  lcd.setCursor(0, 2);
  ;
  lcd.print("2. List Codes");
  lcd.setCursor(0, 3);
  lcd.print("3. TBD");
  String option = getCode(1, 3);
  if (option.equals("1"))
  {
    addCode();
  }
  else if (option.equals("2"))
  {
    listCodesBt("/codes.txt");
  }

  else if (option.equals("3"))
  {
    clearDisplay();
    centerString("Coming Soon", 1);
    delay(1000);
    return;
  }
  else
  {
    clearDisplay();
    centerString("Exiting...", 1);
    failTone();
    delay(1000);
    return;
  }
}

void saveAllCodes(const char *codes[], int count)
{ // Save the generated codes to fill => saveAllCodes(defaultCodes, 50)
  File file = SPIFFS.open("/codes.txt", FILE_WRITE);
  if (!file)
  {
    Serial.println("❌ Failed to open file for writing");
    return;
  }

  for (int i = 0; i < count; i++)
  {
    file.println(codes[i]);
    Serial.printf("✅ Saved: %s\n", codes[i]);
  }

  file.close();
  Serial.println("📄 All codes written to /codes.txt");
}

void listCodes()
{
  File file = SPIFFS.open("/codes.txt", FILE_READ);
  if (!file)
  {
    Serial.println("❌ Failed to open file for reading");
    return;
  }

  Serial.println("📋 Codes in file:");
  while (file.available())
  {
    String line = file.readStringUntil('\n');
    line.trim();
    Serial.println(line);
  }

  file.close();
}

bool isInFile(const char *path, const String &code)
{
  File file = SPIFFS.open(path, FILE_READ);
  if (!file)
  {
    Serial.printf("❌ Failed to open %s\n", path);
    return false;
  }

  while (file.available())
  {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line == code)
    {
      file.close();
      return true;
    }
  }

  file.close();
  return false;
}

void appendToFile(const char *path, const String &code)
{
  File file = SPIFFS.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.printf("❌ Failed to open %s for appending\n", path);
    return;
  }

  file.println(code);
  file.close();
  Serial.printf("✅ Added [%s] to %s\n", code.c_str(), path);
}

bool listCodesBt(const char *path)
{ // call as listCodesBt("/usedCodes.txt")
  File file = SPIFFS.open(path, FILE_READ);
  if (!file)
  {
    Serial.println("❌ Failed to open file for reading");
    return false;
  }

  if (btclassic.hasClient())
  {
    successTone();
    clearDisplay();
    centerString("Sending code", 0);
    centerString("in app", 1);
    Serial.println("Sending code in app.");
    delay(1000);
    btclassic.println("📋 Codes in file:");
    while (file.available())
    {
      String line = file.readStringUntil('\n');
      line.trim();
      btclassic.println(line);
    }

    file.close();
    clearDisplay();
    return true;
  }
  else
  {
    Serial.println("📋 Codes in file:");
    while (file.available())
    {
      String line = file.readStringUntil('\n');
      line.trim();
      Serial.println(line);
    }

    file.close();
    clearDisplay();
    centerString("No Bluetooth", 0);
    centerString("device connected!", 1);
    Serial.println("No devices connected on Bluetooth");
    failTone();
    delay(1200);
    clearDisplay();
    return false;
  }
}


void cleanCodes() {
  if (!SPIFFS.exists("/codes.txt") || !SPIFFS.exists("/usedCodes.txt")) {
    Serial.println("One or both files are missing!");
    return;
  }

  // Read used codes into a set
  std::set<String> usedCodes;
  File usedFile = SPIFFS.open("/usedCodes.txt", "r");
  while (usedFile.available()) {
    String code = usedFile.readStringUntil('\n');
    code.trim();
    if (code.length() > 0) {
      usedCodes.insert(code);
    }
  }
  usedFile.close();

  // Read codes.txt and filter
  File codesFile = SPIFFS.open("/codes.txt", "r");
  std::vector<String> validCodes;
  while (codesFile.available()) {
    String code = codesFile.readStringUntil('\n');
    code.trim();
    if (code.length() > 0 && usedCodes.find(code) == usedCodes.end()) {
      validCodes.push_back(code);
    }
  }
  codesFile.close();

  // Write back the filtered codes
  codesFile = SPIFFS.open("/codes.txt", "w");
  for (auto &code : validCodes) {
    codesFile.println(code);
  }
  codesFile.close();

  Serial.println("codes.txt cleaned!");
}