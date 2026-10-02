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


String code1Array[160] = {
"D9C04D","D0A06D","327155","994DB4","B7C9A3","6C2628","69D32D","59C31C","5AB891","273752",
"7DA777","B6719A","0116D3","049381","3764A7","AA5DD2","C1A5C2","A00B31","9C437A","913A9A",
"17DD2C","B9B8B2","D8CB1A","CA24C5","5BDA52","33C413","6683B7","40733D","38BB21","B7D4D2",
"72533C","91CDA1","C1546C","C7DA37","59A35D","3910D1","877A01","C647C1","01B8CC","C08C83",
"8CDA14","AAB832","20C4C7","C4C91C","C0C7B9","279030","D89B4C","A19CBA","23A1CC","74284B",
"094C19","4307C4","C196AA","35A0C7","3274B3","6A7C12","8C91B9","9C1434","17D3BB","A02106",
"397828","3AB8D5","1C74AC","39B874","30AC8C","81C0D5","75D90C","8DA9BC","745129","5405A5",
"67B4C3","C7C0D9","D1A66C","67D3BB","0B6C23","D29C41","7B6B82","D59A09","01A783","56A1DA",
"2D1B91","CDB94C","C4C1B9","205A71","1C1487","9A0C2B","B0B690","3C0B32","CC31C9","074BB4",
"0A04CA","7280C1","3C87D1","8D4B8C","8C2B89","C3500D","61D32A","4C7B8D","5DC41B","64D401",
"09A4D7","D0A19C","3B4D37","96C7A4","828D14","2BB861","D98B3C","74A1D9","7A06B8","5D40BB",
"0A3B92","C05D87","A43C71","A4B927","91C885","AB9498","C7B124","0C3065","7C6D12","18C43D",
"C8C193","6D3A4C","6DC31C","93A4B3","D3CA38","3381D5","7B1402","D4D0C2","D19CB1","DCB281",
"D807B9","B48209","6D7BD1","810C19","94C273","2D3B7C","7A04A9","65BB1C","CDB7A3","04DA77",
"C91357","74B27C","504C98","968BC9","7B5D32","40A0A3","CD89C1","3287C4","8B2D71","7895B1",
"412D72","D92301","474D98","6B7C94","51A7C9","A13DC7","84D2AB","58845B","C9B4A7","C31A76"
};

String code2Array[160] = {
"8454C4","37A814","B27DC5","5A8445","DDC662","798AD6","37B0D1","8D4C4D","493085","3B786D",
"5364C1","5A1059","71596B","132AD1","919330","C91A76","A4A4C7","B5A0A8","82CB2B","C6DA72",
"07B3B2","013A07","6B71D8","210C78","A4970B","D18141","37C08C","4A4D8B","C7B4B7","972347",
"7C3250","B24D42","C8DD1C","61AB5C","AC06B7","922B54","84C2B8","4092C4","7390B2","C23882",
"7DC828","708A31","1B7D0D","49C114","4D72A9","C2B0DA","2B54C5","B6C0D2","3C38DB","9BA14C",
"AAB0D7","50D8A8","D3A24B","52A8C9","9CD0A7","6B8944","BC21A7","542C7B","C21D9A","57D3D4",
"41C2C3","8B6D35","D7C4C5","A0DB24","C3C4D6","05D207","94B3A0","3D8B82","6C31D9","C308BC",
"58C264","5B8248","6BA2B6","84D385","D4A663","C13D39","3BB6C7","B0B254","62C74B","A6B2C4",
"1C3BB7","A73BCA","58A14C","9270C5","80A46C","B81AC2","D108A2","BB175D","3281C4","A2B8A6",
"29B41D","8D42C1","D1AB94","92B4D3","5873C6","AAB3C1","49A45C","C340DD","5377C9","438D64",
"24A14B","BE8D90","9C5D71","5DC884","C9A0B1","2CD34A","D0D4B2","35D19C","C7C4A1","4C81A2",
"78D32C","974D3B","3B43C2","C3A9B1","A958C0","40D1A6","64C724","B4B238","C3C85A","2D6B8C",
"44BA9D","A2D56C","0C9B54","69D2A1","8C83A7","6B1CA4","C6C5B1","39BA85","7C76D1","A9C78B",
"1D4C8A","C43798","7897A2","D9C253","B78A63","4A97B2","1A4B66","D0B6A8","82531A","33D720"
};

String code3Array[160] = {
"BD34C7","24786B","4AB84A","DA35C6","D7D0D9","3C6A32","D7B6B8","2C7BB5","37DA72","D6C3B4",
"8D3245","C31C0B","85DA16","4DB936","B16C9C","9E30A1","C87332","0C6A53","5B2A39","73D1A0",
"4093C6","D3A73C","B0D478","1DB44A","73C0B7","81ABCC","DC1B4D","2B3A6A","B43DA0","4AB7C1",
"50C38B","D60DB1","613AD0","A5D0C8","19B437","4C71DB","D61A59","C4A706","8259A0","40A8D0",
"8B7741","72C764","B1A6C7","31C642","93C2D8","C8094A","6DC22B","DC0A82","834D26","D38C04",
"03DA96","21478B","6C26A8","4370A5","DBA05B","137DC1","A42D83","5DBD12","03A967","C4B6C8",
"8D1706","9BB0C1","67C81B","50AB72","2C8BD1","8A4D86","DB4867","1C693B","3BA7D0","B89AC0",
"8C0C75","509B32","D8C7C0","57C0A8","2AD3B6","9C41B7","C805CB","3D7062","A28BB3","83B4A1",
"6470BB","C12B49","2CB5C0","8B62A8","D24D37","4386B0","05AD8A","B2097B","17C3C9","92A7D1",
"6D1C3A","A75C2B","C0DD32","7860D1","94C0D8","36A2D1","2A9D83","7B4890","C2406A","873D4B",
"53C801","C9A35D","D2B183","7C7DA3","A8D40B","308A6B","C59037","DB10A3","6A3BD7","4529DB",
"9880C1","C70336","742A31","02C4A9","C1B0A7","7A6D4C","01B47C","3B67AD","AD46C1","61C6A0",
"8C8DA3","97A7C2","B6D87C","C41DA9","0C8743","8B0A44","B4816A","7D94C2","586137","0D91A8",
"7A43C0","8A6B51","4C98B2","C7A4D0","1781B9","8C7D02","D8A32C","317C6B","6A85C0","41A36D"
};

String code4Array[160] = {
"A3D94B","6480D1","B2A1C9","CD0A58","D1C7D4","707A1D","59CB3A","A03CB8","2DB1D7","B4B7C9",
"53A9D1","0D4BA7","96C073","4ACB28","C97A03","D4B762","18A67C","B8D47C","2A40A1","D79109",
"0C96D1","C2B548","7B63A1","924BB7","D20C98","74C832","83AD1C","B71C9B","38A16B","C8CBA2",
"87B61C","41B82D","5C0A7A","A1C7B9","D38101","2C9C73","C8B174","AD14D8","64C71D","B2C4A1",
"9A3824","C9C658","18B3A7","D4C073","0A57B8","C1A78D","839B07","7B40C4","C6A2B1","1A64D0",
"78C43A","C20867","403879","A61DA4","7C518B","B1C4A7","1D2D8C","9C7D40","8A31C7","63B21D",
"4A81B6","BE30C7","0C4B92","A4C5B1","19C6A7","63C740","7DB04C","4DA87C","C3A18D","7CD0C4",
"4C3A81","C170B6","862741","7381D0","A63D7C","C4DB13","0B71C6","9A30B7","D2C41B","872DC1",
"04C1A9","B347A8","18DA93","C7C981","953C7A","3A0B69","4C8A75","A19D0C","C60438","8D91B7",
"1DB46C","78A6D4","C2B61D","B57C8A","03A7C8","89D14C","3678A1","D4C7A9","9B4A6D","64D8C1",
"31C9A5","A47B63","4B3A1C","D6B7A1","0D3C82","C4B305","1C6BDA","B40C8D","5842A0","C2416C",
"7A91C8","3D4C67","C7A130","6A3B47","4A6D1C","A7643C","C9B2A4","84DA67","1AC5A4","93C04A",
"7D81C0","A4B79C","5D3A48","C140D8","2A873C","B7C04A","03B8D1","C2C18A","7C1A4D","A13C6B",
"4CBD01","C3A72D","9A1C4B","6A79D3","30AB6D","C4C79D","8A31B6","79D3C4","1B8C07","D0A68B",
"4C931A","C12B8D","7C39D8","A8C4B0","13B67C","C8A173","0A3C72","6C81A7","81C7B0","3B47C9"
};



  
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
#define LOCK2 13
#define LOCK3 14
#define LOCK4 15
#define BUZZER 23

// BLE Configuration
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
std::vector<String> partition1Codes, partition2Codes, partition3Codes, partition4Codes, partition5Codes, partition6Codes, partition7Codes, partition8Codes;
BLEServer *pServer = NULL;
BLEService *pService = nullptr;
BLECharacteristic *pCharacteristic = NULL;

String agentId = "default", passKey = "1234", ocuppiedPartitions = "0", overCode = "654321", agentIdFromNVS, receivedData, response;
String partition1Master = "CBA451", partition2Master = "CBA452", partition3Master = "CBA453", partition4Master = "CBA454";
char messageBuffer[2500];

const int r1 = 39, r2 = 34, r3 = 35, r4 = 32, c1 = 33, c2 = 25, c3 = 26, c4 = 27;
const byte ROWS = 4; // rows
const byte COLS = 4; // columns

// Global variables (declare outside of any function, e.g., at the top of your sketch)
unsigned long lastInteractionTime = 0;
unsigned long lastBatteryUpdate = 0;
unsigned long lastServerUpdate = 0;
const int cabinetNumber = 4;    // Cabinets in the box
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
void codeSuccess(int lock);
void launchSettings();
void addCode();
void cleanCodes();
bool appendCode(const String &code);
bool codeExistsSPIFF(const String &code);
void saveAllPartitionsToSPIFFS();
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
  // cleanCodes();
  // SPIFFS.format();
  // delay(1000);
  WiFi.begin(SSID, PASSWORD);       //*To Remove
  lcd.init();
  lcd.backlight();
  keys.begin();
  // btclassic.begin("MyBox");
  pinMode(BUZZER, OUTPUT);
  pinMode(LOCK1, OUTPUT);
  pinMode(LOCK2, OUTPUT);
  pinMode(LOCK3, OUTPUT);
  pinMode(LOCK4, OUTPUT);
  centerString("MyBox", 1);
  // saveAllPartitionsToSPIFFS();
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


  if (millis() - lastInteractionTime > 10000) { // 30 seconds of inactivity
    if (isBacklightOn) {
      lcd.noBacklight();
      isBacklightOn = false;
    }
  }


  else{
    lcd.backlight();
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

    else if (code.equals("CBA451") || isInFile("/codes1.txt", code)) {
      if (!code.equals("CBA451")) {
        appendToFile("/usedCodes.txt", code);
      }
      codeSuccess(LOCK1); // You can add delay/return inside this
      right_code = code; // Store the right code
    }

    else if (code.equals("CBA452") || isInFile("/codes2.txt", code)) {
      if (!code.equals("CBA452")) {
        appendToFile("/usedCodes.txt", code);
      }
      codeSuccess(LOCK2); // You can add delay/return inside this
      right_code = code; // Store the right code
    }

    else if (code.equals("CBA453") || isInFile("/codes3.txt", code)) {
      if (!code.equals("CBA453")) {
        appendToFile("/usedCodes.txt", code);
      }
      codeSuccess(LOCK3); // You can add delay/return inside this
      right_code = code; // Store the right code
    }

    else if (code.equals("CBA454") || isInFile("/codes4.txt", code)) {
      if (!code.equals("CBA454")) {
        appendToFile("/usedCodes.txt", code);
      }
      codeSuccess(LOCK4); // You can add delay/return inside this
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

void codeSuccess(int lock)
{
  String msg;
  switch (lock){
    case 12:
     msg = "Opening Box 1";
     break;
    case 13:
     msg = "Opening Box 2";
     break;
    case 14:
     msg = "Opening Box 3";
     break;
    case 15:
     msg = "Opening Box 4";
     break;
  }
 
  successTone();
  printMessage(msg);
  digitalWrite(lock, 1);
  delay(3000);
  digitalWrite(lock, 0);
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

void saveAllPartitionsToSPIFFS()
{
    struct Partition {
        const char* filename;
        String* codes;
    };

    // List of all partitions
    Partition partitions[] = {
        { "/codes1.txt", code1Array },
        { "/codes2.txt", code2Array },
        { "/codes3.txt", code3Array },
        { "/codes4.txt", code4Array }
    };

    // Loop through and save each partition
    for (int p = 0; p < 4; p++)
    {
        const char* filename = partitions[p].filename;
        String* codes = partitions[p].codes;

        File file = SPIFFS.open(filename, FILE_WRITE);
        if (!file)
        {
            Serial.printf("Error opening %s for writing\n", filename);
            continue;
        }

        for (int i = 0; i < 50; i++)
        {
            file.println(codes[i]);
        }

        file.close();
        Serial.printf("All codes written to %s\n", filename);
    }

    Serial.println("Finished writing all partitions.");
}


void listCodes(const char *path = "/codes.txt")
{
  File file = SPIFFS.open(path, FILE_READ);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
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
    Serial.println("Failed to open file for writing");
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
  if (voltage >= 16.4) return 100;
  if (voltage <= 12.0) return 0;
  return (int)(((voltage - 12) / (16- 12)) * 100);
}

// Task that runs on core 0
void batteryTask(void *pvParameters) {
    float voltage = readBatteryVoltage();
    int percent = batteryPercentage(voltage);
    if (percent < hpercent || (hpercent+10 < percent)) {
      hpercent = percent; // Update global variable for battery percentage
    }
    Serial.printf("Battery: %.2f V (%d%%)\n", voltage, percent);
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
