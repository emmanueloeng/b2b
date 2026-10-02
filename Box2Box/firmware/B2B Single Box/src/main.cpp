#include <Arduino.h>
#include <Adafruit_Keypad.h>
#include <HTTPClient.h>
#include <set>
#include <ArduinoJson.h>
#include <vector>
#include <LiquidCrystal_I2C.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <algorithm>
#include <vector>
#include <WiFi.h>
#include "esp_bt_device.h"
#include <WiFiClientSecure.h>
#include <BluetoothSerial.h>
#include <FS.h>
#include <SPIFFS.h>


char codes[50][7]; // 50 codes, each up to 6 chars + null terminator
char usedCodes[50][7];

// WiFi credentials
// const char *SSID = "STARLINK";
// const char *PASSWORD = "";

const char *SSID = "STARLINK";
const char *PASSWORD = "";

// API endpoint
const char* serverName = "https://bacobagtest.inhouse.codes/api/device-status";
const char* apiKey = "6e87f94a12b54f5a80be2468c7d1cf3f";
WiFiClientSecure client;


const char *defaultCodes[50] = {
    "A4C7B2", "196C5A", "A7C43B", "5823B1", "C91AB6", "7A6C98", "1C5B7A", "3B124C", "75919A", "C1B8AC",
    "A4C58B", "839C2B", "9A8C7B", "45B1C7", "2C8B13", "C51A97", "62B8C1", "A94C1B", "718C32", "7CB7A9",
    "A9B883", "251C98", "C1B4A7", "58C3A1", "3C92B7", "7B4186", "C82AB3", "1A5C34", "C3B27A", "6B8521",
    "4CA219", "A1B97C", "7C3982", "59ACB7", "C8B4A1", "7B2A93", "8B4C1A", "3C1A97", "62B9C3", "C8A471",
    "5B9C13", "7C2A98", "C1B52A", "A7C9B3", "34B8C1", "7C7A95", "1B4C96", "8A6C2B", "3C75A9", "B1C4A7"};

  
const char* root_ca = "-----BEGIN CERTIFICATE-----\n" \
"MIIGWzCCBUOgAwIBAgIRAM5UY0o9DCFtJG1yIzBXTX4wDQYJKoZIhvcNAQELBQAw\n" \
"gY8xCzAJBgNVBAYTAkdCMRswGQYDVQQIExJHcmVhdGVyIE1hbmNoZXN0ZXIxEDAO\n" \
"BgNVBAcTB1NhbGZvcmQxGDAWBgNVBAoTD1NlY3RpZ28gTGltaXRlZDE3MDUGA1UE\n" \
"AxMuU2VjdGlnbyBSU0EgRG9tYWluIFZhbGlkYXRpb24gU2VjdXJlIFNlcnZlciBD\n" \
"QTAeFw0yNTAyMDgwMDAwMDBaFw0yNjAyMDgyMzU5NTlaMCQxIjAgBgNVBAMTGWJh\n" \
"Y29iYWd0ZXN0LmluaG91c2UuY29kZXMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAw\n" \
"ggEKAoIBAQCWwKIIY3ttw6Iss0/hPPUQ/I0AY6FkBvXNQWQndlYJHLRquoNQkW7k\n" \
"PZRPQH5363q65lamAQdYWVNed+zO3avM9z0onqZMD09/oEP/pFzja/KftNJq/UV8\n" \
"mq8DrdOa2svxJS93UlcF5m5K/ZxBzqapGKG8TQtxoykCBIWGFDfsl0NohhEGj8s9\n" \
"4juvNpkwLTWSsxGE4R5MuFWp2LDKz3f3GBbjyB95YPYckhzSFrKRsvq1ZyFQ8ZI8\n" \
"58dgSy8UaFauCXAV95vVA+8yE3+LcGGaYV3IYHFwmEYtEb0sjD9xhcqkPgL62Wjs\n" \
"DwHYArjmPslwM8yoFQMbowhQ+AO07RYDAgMBAAGjggMaMIIDFjAfBgNVHSMEGDAW\n" \
"gBSNjF7EVK2K4Xfpm/mbBeG4AY1h4TAdBgNVHQ4EFgQUmrDxlcfm2rJWJqU58mzq\n" \
"zBW7oqwwDgYDVR0PAQH/BAQDAgWgMAwGA1UdEwEB/wQCMAAwHQYDVR0lBBYwFAYI\n" \
"KwYBBQUHAwEGCCsGAQUFBwMCMEkGA1UdIARCMEAwNAYLKwYBBAGyMQECAgcwJTAj\n" \
"BggrBgEFBQcCARYXaHR0cHM6Ly9zZWN0aWdvLmNvbS9DUFMwCAYGZ4EMAQIBMIGE\n" \
"BggrBgEFBQcBAQR4MHYwTwYIKwYBBQUHMAKGQ2h0dHA6Ly9jcnQuc2VjdGlnby5j\n" \
"b20vU2VjdGlnb1JTQURvbWFpblZhbGlkYXRpb25TZWN1cmVTZXJ2ZXJDQS5jcnQw\n" \
"IwYIKwYBBQUHMAGGF2h0dHA6Ly9vY3NwLnNlY3RpZ28uY29tMEMGA1UdEQQ8MDqC\n" \
"GWJhY29iYWd0ZXN0LmluaG91c2UuY29kZXOCHXd3dy5iYWNvYmFndGVzdC5pbmhv\n" \
"dXNlLmNvZGVzMIIBfgYKKwYBBAHWeQIEAgSCAW4EggFqAWgAdgCWl2S/VViXrfdD\n" \
"h2g3CEJ36fA61fak8zZuRqQ/D8qpxgAAAZTmPuG2AAAEAwBHMEUCIQCL88/x0qPz\n" \
"t3q/3TVWz5NXrqa/AHr3F6plgZL5CgWKPwIgRJXG0wNyYzUiaBPEjkNi9tbGfNaP\n" \
"OGgY8buqAhTPSwIAdgAZhtTHKKpv/roDb3gqTQGRqs4tcjEPrs5dcEEtJUzH1AAA\n" \
"AZTmPuFIAAAEAwBHMEUCIQCheRlAq6z3eso/ASXOA84hkljEeuVYoFcmUD75lr9h\n" \
"/gIgAuCu2p7BNjiwssK92wL/4z4ZExaCCKYlEqW9czTxqXIAdgDLOPcViXyEoURf\n" \
"W8Hd+8lu8ppZzUcKaQWFsMsUwxRY5wAAAZTmPuHnAAAEAwBHMEUCIQDbPt611nQL\n" \
"QUQNEsiSoHjAXXmEc5n7y3reX58SZbBNGgIgQkQ/OxISJWKLJBYZ189wzha6PaTB\n" \
"6XqcXp0JjYx2CXswDQYJKoZIhvcNAQELBQADggEBABvEU2BNSU8S+sVc7YeOV9xY\n" \
"mJvWmPMvCm2HuGQ08x0ltklRyIaZ5MP7R2j5y3kgQIcU4si+V69koAyHmWQxx16Z\n" \
"do3WgxU22jbiPijXlCqFk39iQW8RAI27LGid6TA8llj1qYgX7mxAIutSQgytpcHJ\n" \
"gM+9+B9RjyyzXSw5rl+uXieI932zAWQwYje+kRq5j4dMQ1AXvm7lsViRFTsAoFS7\n" \
"+SBL9Nm7+uaAbeHFgbmouDPHIli/QTAq1swCb9PTR9HW+RBdQNhPvUMi/i2uenJI\n" \
"w0uj51oPfkzzpvUDfoyrrWBt3AR1kaP6nPFJinYIPd4UhaTa9BDASHZvgI9VYf4=\n" \
"-----END CERTIFICATE-----\n";


#define BATTERY_PIN 36 // ADC input pin
#define R1 100000.0
#define R2 33000.0


#define LCD_WIDTH 20
#define LCD_HEIGHT 4
#define LOCK1 12
#define BUZZER 23

// BLE Configuration
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
std::vector<String> partition1Codes, partition2Codes, partition3Codes, partition4Codes, partition5Codes, partition6Codes, partition7Codes, partition8Codes;
BLEServer *pServer = NULL;
BLEService *pService = nullptr;
BLECharacteristic *pCharacteristic = NULL;

String agentId = "default", passKey = "1234", ocuppiedPartitions = "0", overCode = "654321", agentIdFromNVS, receivedData, response;
char messageBuffer[2500];

const int r1 = 39, r2 = 34, r3 = 35, r4 = 32, c1 = 33, c2 = 25, c3 = 26, c4 = 27;
const byte ROWS = 4; // rows
const byte COLS = 4; // columns

// Global variables (declare outside of any function, e.g., at the top of your sketch)
unsigned long lastInteractionTime = 0;
unsigned long lastBatteryUpdate = 0;
unsigned long lastServerUpdate = 0;
bool isBacklightOn = true;
bool deviceConnected = false;
bool bleInitialized = false;
int hpercent = 100; // Global variable for battery percentage
int unlocked = 0; // Global variable to track unlock status
String right_code = "";
unsigned long lastCorrectCode = 0; // To track the last correct code entry time 

struct CodeInput {
  String code = "";
  int length = 6;
  int row = 1;
  bool done = false;
  bool active = false; // To track if we’re in input mode
}; 
CodeInput codeInput;


// define the symbols on the buttons of the keypads
char matkeys[ROWS][COLS] = {
    {'1', '4', '7', '*'},
    {'2', '5', '8', '0'},
    {'3', '6', '9', '#'},
    {'A', 'B', 'C', 'D'}};

byte rowPins[COLS] = {c1, c2, c3, c4};
byte colPins[ROWS] = {r1, r2, r3, r4};

// Component Initializations
Adafruit_Keypad keys = Adafruit_Keypad(makeKeymap(matkeys), colPins, rowPins, ROWS, COLS);
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LiquidCrystal_I2C lcd(0x27, LCD_WIDTH, LCD_HEIGHT); // Adjust I2C address if needed
BluetoothSerial btclassic;

// Callback Class for Handling Received Data
class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    String rxValue = String(pCharacteristic->getValue().c_str()); // Get the raw data
    if (rxValue.length() > 0)
    {
      // Arduino String
      for (int i = 0; i < rxValue.length(); i++)
      {
        receivedData += rxValue[i];
      }
    }
  }
};

// BLE Server Callbacks for Connection Management
class MyServerCallbacks : public BLEServerCallbacks
{
  String getInterfaceMacAddress(esp_mac_type_t interface)
  {
    String mac = "";
    unsigned char mac_base[6] = {0};
    if (esp_read_mac(mac_base, interface) == ESP_OK)
    {
      char buffer[18]; // 6*2 characters for hex + 5 characters for colons + 1 character for null terminator
      sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
      mac = buffer;
    }
    return mac;
  }
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
    String chipID = getInterfaceMacAddress(ESP_MAC_BT);
    pCharacteristic->setValue(chipID.c_str()); // Send chip ID
    pCharacteristic->notify();
  }

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
    BLEDevice::startAdvertising(); // Restart advertising on disconnect
  }
};
String getDeviceID();
void sendDeviceStatus(int batteryPercent, int deviceUnlocked);
void handleCodeInput(CodeInput &input);
float readBatteryVoltage();
int batteryPercentage(float voltage);
void batteryTask(void *pvParameters);
void updateCodes();
void processData();
void handleBLE();
String readEntireFile(const char *path = "/response.txt");
void centerString(String message, int row);
String getInterfaceMacAddress(esp_mac_type_t interface);
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
void saveToFile(const char *path);
void listCodes();
bool isInFile(const char *path, const String &code);
void appendToFile(const char *path, const String &code);
bool listCodesBt(const char *path);

void setup()
{
  Serial.begin(115200);
  analogReadResolution(12);  // 12-bit ADC
  analogSetAttenuation(ADC_11db); // Allows reading up to ~3.3v
  SPIFFS.begin(true);
  cleanCodes();
  delay(1000);
  WiFi.begin(SSID, PASSWORD);
  // saveAllCodes(defaultCodes, 50);
  // listCodes();
  lcd.init();
  lcd.backlight();
  keys.begin();
  // btclassic.begin("MyBox");
  pinMode(BUZZER, OUTPUT);
  pinMode(LOCK1, OUTPUT);
  centerString("MyBox", 1);
  digitalWrite(BUZZER, 1);
  delay(1500);
  digitalWrite(BUZZER, 0);
  lcd.clear();
}

void loop() {

  static bool initialized = false;
  if(millis()-lastBatteryUpdate > 1000) {     // Update battery status every second
  batteryTask(NULL); // Call battery task to update battery status  
  lastBatteryUpdate = millis();
  }

  if (millis() - lastInteractionTime > 10000) {
    uint64_t wakeup_mask = 0;
  for (int i = 0; i < 4; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
    wakeup_mask |= 1ULL << rowPins[i];
  }

  esp_sleep_enable_ext1_wakeup(wakeup_mask, ESP_EXT1_WAKEUP_ANY_LOW);
}

  // Go into deep sleep (execution stops here!)
  esp_deep_sleep_start();


  if (millis() - lastInteractionTime > 10000) { // 30 seconds of inactivity
    if (isBacklightOn) {
      lcd.noBacklight();
      isBacklightOn = false;
    }
  }


  else{
    lcd.backlight();
  }

    if (millis() - lastCorrectCode > 300000){
    right_code = "";
    lastCorrectCode = 0; // Reset after 5 minutes
  }

  if (millis() - lastServerUpdate > 10000) { // Update server every 30 seconds
    sendDeviceStatus(hpercent, unlocked); // Example last unlock time     
    lastServerUpdate = millis();
  }
  if (!initialized) {
    lcd.clear();
    centerString("Enter Lock Code", 0);
    codeInput = CodeInput();      // Reset all fields
    codeInput.active = true;      // Begin input mode
    initialized = true;
  }

  handleCodeInput(codeInput);

  if (codeInput.done) {
    String code = codeInput.code;

    if (code.length() < 6) {
      clearDisplay();
      printMessage("Incomplete Input!");
      failTone();
      delay(1000);
    }

    else if (isInFile("/usedCodes.txt", code) && (code != right_code)) {
      lcd.clear();
      centerString("Code Used", 0);
      centerString("Already!", 1);
      failTone();
      delay(1000);
    }

    else if (code.equals("CBA456") || isInFile("/codes.txt", code) || (right_code == code)) {
      if (!code.equals("CBA456")) {
        appendToFile("/usedCodes.txt", code);
      }
      codeSuccess(); // You can add delay/return inside this
      right_code = code; // Store the right code
    }

    else if (code.equals("246BAA")) {
      lcd.clear();
      centerString("Launching Settings", 1);
      successTone();
      delay(1000);
      launchSettings();
    }

    else {
      printMessage("Wrong Code!");
      failTone();
      delay(1000);
    }

    // Reset state for next input
    initialized = false;
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
  int spaces = (LCD_WIDTH - messageLength) / 2;
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
  delay(3000);
  digitalWrite(LOCK1, 0);
  unlocked = 1;
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
    Serial.println("Code already exists.");
    return false;
  }

  File file = SPIFFS.open("/codes.txt", FILE_APPEND);
  if (!file)
  {
    Serial.println("Failed to open file for appending");
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
  lcd.print("3. Update Device");
  String option = getCode(1, 3);
  if (option.equals("1"))
  {
    addCode();
  }

  else if (option.equals("2"))
  {
    clearDisplay();
    centerString("Codes to send", 0);
    lcd.setCursor(0, 1);
    lcd.print("1. Available codes");
    lcd.setCursor(0, 2);
    lcd.print("2. Used codes");
    String listOption = getCode(1, 3);
    if (listOption.equals("1"))
    {
      clearDisplay();
      centerString("Available Codes", 0);
      listCodesBt("/codes.txt");
      delay(500);
      clearDisplay();
    }
    else if (listOption.equals("2"))
    {
      clearDisplay();
      centerString("Used Codes", 0);
      listCodesBt("/usedCodes.txt");
      delay(500);
      clearDisplay();
    }
    else
    {
      clearDisplay();
      centerString("Invalid Option", 0);
      failTone();
      delay(1000);
      return;
    }
  }

  else if (option.equals("3"))
  {
    clearDisplay();
    centerString("Update Device", 1);
    delay(1000);
    updateCodes();
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

void listCodes(const char *path = "/codes.txt")
{
  File file = SPIFFS.open(path, FILE_READ);
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

void cleanCodes()
{
  if (!SPIFFS.exists("/codes.txt") || !SPIFFS.exists("/usedCodes.txt"))
  {
    Serial.println("One or both files are missing!");
    return;
  }

  // Read used codes into a set
  std::set<String> usedCodes;
  File usedFile = SPIFFS.open("/usedCodes.txt", "r");
  while (usedFile.available())
  {
    String code = usedFile.readStringUntil('\n');
    code.trim();
    if (code.length() > 0)
    {
      usedCodes.insert(code);
    }
  }
  usedFile.close();

  // Read codes.txt and filter
  File codesFile = SPIFFS.open("/codes.txt", "r");
  std::vector<String> validCodes;
  while (codesFile.available())
  {
    String code = codesFile.readStringUntil('\n');
    code.trim();
    if (code.length() > 0 && usedCodes.find(code) == usedCodes.end())
    {
      validCodes.push_back(code);
    }
  }
  codesFile.close();

  // Write back the filtered codes
  codesFile = SPIFFS.open("/codes.txt", "w");
  for (auto &code : validCodes)
  {
    codesFile.println(code);
  }
  codesFile.close();

  Serial.println("codes.txt cleaned!");
}

void saveToFile(const char *path)
{
  File file = SPIFFS.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  file.print(receivedData);
  Serial.print("Response Saved!");
  file.close();
  Serial.println("All codes written to /response.txt");
}

String readEntireFile(const char *path)
{
  File file = SPIFFS.open(path, FILE_READ);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return "";
  }

  String content = "";

  while (file.available())
  {
    content += (char)file.read(); // Read byte-by-byte and append
  }

  file.close();
  return content;
}

String getInterfaceMacAddress(esp_mac_type_t interface)
{
  String mac = "";
  unsigned char mac_base[6] = {0};
  if (esp_read_mac(mac_base, interface) == ESP_OK)
  {
    char buffer[18]; // 6*2 characters for hex + 5 characters for colons + 1 character for null terminator
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
    mac = buffer;
  }
  return mac;
}

void handleBLE()
{
  if (!bleInitialized)
  {
    BLEDevice::init("bacobag_demo2");
    bleInitialized = true;
  }

  if (pServer)
  {
    BLEDevice::stopAdvertising();
    pServer->removeService(pService); // Remove old service
  }

  // Create new BLE Server (if needed)
  if (!pServer)
  {
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
  }

  // Create and start a new service
  pService = pServer->createService(SERVICE_UUID);

  // Create a new characteristic
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Add Descriptor for notifications
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Restart advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x00);
  BLEDevice::startAdvertising();
}

void updateCodes()
{
  /* This function is triggered whenever the device starts up with agentId of default */
  handleBLE();

  unsigned long startMillis = millis(); // Start time for Bluetooth connection wait
  const unsigned long timeout = 180000; // 3-minute timeout

  while (!deviceConnected)
  { // Wait for Bluetooth connection
    if (millis() - startMillis >= timeout)
    {
      clearDisplay();
      centerString("TIMEOUT!", 0);
      centerString("NO CONNECTION", 1);
      delay(2000);
      return; // Exit function after timeout
    }

    clearDisplay();
    centerString("CONNECT DEVICE TO", 0);
    centerString("BLUETOOTH", 1);
    delay(1000);
    clearDisplay();
    delay(300);
  }

  while (deviceConnected)
  {
    unsigned long codeStartMillis = millis(); // Start time for user input timeout
    String code = "";

    while (millis() - codeStartMillis < timeout)
    { // Wait for user input for 1 min
      clearDisplay();
      centerString("ENTER 1234", 0);
      centerString("TO CONTINUE", 1);
      lcd.setCursor(8, 2);

      code = getCode(4, 2); // Wait for user input

      if (code.length() > 0)
      { // If user enters anything, reset timer
        codeStartMillis = millis();
      }

      if (code.equals("1234"))
      {
        break; // Exit the timeout loop if correct code is entered
      }
    }

    if (!code.equals("1234"))
    { // If timeout occurs (no input for 1 minute)
      clearDisplay();
      centerString("TIMEOUT!", 0);
      centerString("NO INPUT", 1);
      delay(2000);
      return; // Exit function due to inactivity
    }

    // Proceed if the correct code is entered
    clearDisplay();
    centerString("CONTINUE SETUP", 0);
    centerString("IN MOBILE APP", 1);
    delay(2000);
    clearDisplay();

    // Initiate 10-second wait here
    unsigned long prevMillis = millis();
    const unsigned long wTime = 10000;

    while (millis() - prevMillis < wTime)
    {
      clearDisplay();
      centerString("WAITING.", 1);
      delay(500);
      clearDisplay();
      centerString("WAITING..", 1);
      delay(500);
      clearDisplay();
      centerString("WAITING...", 1);
      delay(500);
    }

    clearDisplay();
    if (receivedData.length() > 100)
    { // Check if data is received,then save to file
      centerString("DEVICE UPDATE", 0);
      centerString("SUCCESSFUL", 1);
      Serial.print(receivedData); // For debugging, delete in production
      saveToFile("/response.txt");
      delay(2000);
    }
    else
    {
      centerString("INCOMPLETE", 0);
      centerString("TRANSMISSION!", 1);
      failTone();
      delay(2000);
      clearDisplay();
      centerString("RESEND DATA", 1);
      delay(1000);
      clearDisplay();
    }

    processData();
    break;
  }
  processData();
}

void saveCodesToFile(const std::vector<String> &codes, const char *path = "/codes.txt")
{
  File file = SPIFFS.open(path, FILE_WRITE);  // FILE_WRITE truncates old contents
  if (!file)
  {
    Serial.println("❌ Failed to open file for writing");
    return;
  }

  for (const String &code : codes)
  {
    file.println(code);  // Write each code on its own line
  }

  file.close();
  Serial.println("Codes saved to file");
}


void processData() {
  // Lambda function to convert JsonArray to std::vector<String>
  auto jsonArrayToVector = [](JsonArray jsonArray) {
    std::vector<String> codes;
    for (String code : jsonArray) {
      codes.push_back(code);
    }
    return codes;
  };

  response = readEntireFile("/response.txt");
  if (response.length() < 10) {
    clearDisplay();
    centerString("NO DATA TO PROCESS", 0);
    delay(2000);
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  if (doc["partitions"].containsKey("partition 1")) {
    partition1Codes = jsonArrayToVector(doc["partitions"]["partition 1"]["codes"].as<JsonArray>());
    saveCodesToFile(partition1Codes, "/codes.txt");
  }
}


float readBatteryVoltage() {
  int raw = analogRead(BATTERY_PIN);
  float voltage = (raw / 4095.0) * 3.3;  // ADC to actual voltage at divider output
  float batteryVoltage = voltage * ((R1 + R2) / R2);
  return batteryVoltage;
}

int batteryPercentage(float voltage) {
  if (voltage >= 12.6) return 100;
  if (voltage <= 9.0) return 0;
  return (int)(((voltage - 9.0) / (12.6 - 9.0)) * 100);
}

// Task that runs on core 0
void batteryTask(void *pvParameters) {
    float voltage = readBatteryVoltage();
    int percent = batteryPercentage(voltage);
    if (percent < hpercent || (hpercent+10 < percent)) {
      hpercent = percent; // Update global variable for battery percentage
    }
    Serial.printf("🔋 Battery: %.2f V (%d%%)\n", voltage, percent);
    // lcd.setCursor(0, 3); // Line 4 (index 3)
    // lcd.print("Battery:     ");
    lcd.setCursor(16, 3);
    lcd.printf("%3d%%", hpercent);
    vTaskDelay(pdMS_TO_TICKS(100)); // Every 0.1second

  
}


void handleCodeInput(CodeInput &input) {
  if (!input.active || input.done) return;

  keys.tick();
  while (keys.available()) {
    keypadEvent e = keys.read();
    char key = (char)e.bit.KEY;

    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      playTone(BUZZER, 1000, 100);
      lastInteractionTime = millis(); // Reset inactivity timer
      isBacklightOn = true; // Turn on backlight on interaction

      if (key == '#') {
        if (!input.code.isEmpty()) {
          input.code.remove(input.code.length() - 1);
        }
      } else if (key == 'D') {
        input.done = true;
      } else if (input.code.length() < input.length) {
        input.code += key;
      }

      // Update LCD
      lcd.setCursor(1, input.row);
      lcd.print("                ");
      lcd.setCursor(1, input.row);
      lcd.print(input.code);
    }
  }

  if (input.code.length() >= input.length) {
    input.done = true;
  }
}



void sendDeviceStatus(int batteryPercent, int deviceUnlocked) {
  WiFiClientSecure client;
  client.setInsecure(); // For testing without CA cert. Use setCACert(root_ca) in production.

  HTTPClient http;
  http.begin(client, serverName);

  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-KEY", apiKey);

  String payload = "{";
  payload += "\"device_id\":\"28:56:2F:4B:4A:3E\",";
  payload += "\"battery_percentage\":" + String(batteryPercent) + ",";
  payload += "\"last_unlock_time\":\"" + String(deviceUnlocked) + "\"";
  payload += "}";

  Serial.println("Payload: " + payload);

  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    Serial.println("Response: " + http.getString());
  } else {
    Serial.printf("HTTP POST failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
  }

  http.end();
  unlocked = 0;
}
