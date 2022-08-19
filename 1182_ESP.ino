#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

char ssid[] = "Basement";
char password[] = "narahari";

// For HTTP
// WiFiClient client;

// For HTTPS
WiFiClientSecure client;

// base URL
#define baseURL "api.weather.gov"

// site fingerprint for HTTPS *changes every few months* (site certificate > details > properties only) 
//#define SITE_FINGERPRINT "DA 78 F0 6D FF B7 3B 2F FE 96 70 6A 49 76 85 A1 F9 38 B8 0C"

// Root Certificate details that can be used to verify certs further down the chain. Never changes.
// R means raw string; removes need for escape slashes.
static const char rootCert[] PROGMEM = R"EOF(-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----)EOF";
X509List cert(rootCert);
const unsigned int rootCertLen = 947;

//api.weather.gov responds with GMT so offset isn't needed.
//const long UTCOffset = -5 * 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  Serial.begin(9600);
  // Stores the string in Flash rather than RAM; saves space for the api response
  Serial.print(F("Starting"));
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Connect to Wifi network:
  Serial.print(F("Connecting to Wifi: "));
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("");
  Serial.println(F("Connected"));
  Serial.println(F("Current IP address: "));
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  // Don't check fingerprint
   client.setInsecure();

  // Check the fingerprint
  //client.setFingerprint(SITE_FINGERPRINT);

  // Check Root Cert
  //client.setTrustAnchors(&cert);

  timeClient.begin();
  //getForecast();

  pinMode(2, OUTPUT); // Trigger pin for Arduino Nano
}

// Returns the date and time of the next predicted snowfall.
String getForecast() {
  // Connect to server (HTTP uses port 80)
  if (!client.connect(baseURL, 443)) {
    Serial.println(F("Failed to Connect"));
    return "";
  }

  // Yield to scheduler
  yield();

  // Send HTTP GET request
  client.print(F("GET "));
  // Check OSU Forecast
  client.print(F("/gridpoints/ILN/83,82/forecast"));
  // HTTP Version
  client.println(F(" HTTP/1.1"));

  //Headers for weather.gov
  client.print(F("Host: "));
  client.println(baseURL);
  client.println(F("Cache-Control: no-cache"));
  client.println(F("User-Agent: (ESP8266, mithrak8022@gmail.com)"));

  if (client.println() == 0) {
    Serial.println(F("Request Failed"));
    return "";
  }
  // Check Status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected Response: "));
    Serial.println(status);
    return "";
  }

  // Ignore Headers
  char headerEnds[] = "\r\n\r\n";
  if (!client.find(headerEnds)) {
    Serial.println(F("Invalid response"));
    return "";
  }

  // If bad characters are read before the JSON, throw them out.
  while (client.available() && client.peek() != '{') {
    char c = 0;
    client.readBytes(&c, 1);
    Serial.print(c);
    Serial.println("BAD");
  }

  //https://arduinojson.org/v6/assistant/
  //Use assistant to calculate allocation size for api response. 
  DynamicJsonDocument doc(16384);

  DeserializationError error = deserializeJson(doc, client);

  if (!error) {
    // Goes through the 14 forecasts over a week and returns the start time of the next snowfall.
    String forecast, snowTime;
    for (int i = 0; i < 14; i++) {
      forecast = String(doc["properties"]["periods"][i]["detailedForecast"]);
      //Serial.println(forecast);
      if (forecast.indexOf("snow") != -1) {
        snowTime = String(doc["properties"]["periods"][i]["startTime"]);
        break;
      }
    }
    return snowTime;
  }
  else {
    Serial.print(F("deserializeJson() error: "));
    Serial.println(error.f_str());
    return "";
  }
}

// Returns the current date and time.
String getUTCDateTime() {
  if (!client.connect("worldtimeapi.org", 443)) {
    Serial.println(F("Failed to Connect"));
    return "";
  }

  yield();

  // Send HTTP GET request
  client.print(F("GET "));
  // Check OSU Forecast
  client.print(F("/api/timezone/America/New_York"));
  // HTTP Version
  client.println(F(" HTTP/1.1"));

  if (client.println() == 0) {
    Serial.println(F("Request Failed"));
    return "";
  }
  // Check Status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected Response: "));
    Serial.println(status);
    return "";
  }

   // Ignore Headers
  char headerEnds[] = "\r\n\r\n";
  if (!client.find(headerEnds)) {
    Serial.println(F("Invalid response"));
    return "";
  }

  // If bad characters are read before the JSON, throw them out.
  while (client.available() && client.peek() != '{') {
    char c = 0;
    client.readBytes(&c, 1);
    Serial.print(c);
    Serial.println("BAD");
  }

  DynamicJsonDocument dtime(1024);

  DeserializationError error = deserializeJson(dtime, client);

  if (!error) {
    // Prints the current UTC Date and Time
    String date = String(dtime["datetime"]);
    date = date.substring(0,19) + date.substring(26);
    Serial.println(date);
    return date;
  }
  else {
    Serial.print(F("deserializeJson() error: "));
    Serial.println(error.f_str());
    return "";
  }
}

// Compares the predicted snowfall time to the current time.
// Returns a 1 if UTCTime > weatherTime.
// 2022-04-18T18:00:00-04:00
int compareDate(String weatherTime, String UTCTime) {
  int year1   = weatherTime.substring(0,4).toInt(),   year2   = UTCTime.substring(0,4).toInt();
  int month1  = weatherTime.substring(5,7).toInt(),   month2  = UTCTime.substring(5,7).toInt();
  int day1    = weatherTime.substring(8,10).toInt(),  day2    = UTCTime.substring(8,10).toInt();
  int hour1   = weatherTime.substring(11,13).toInt(), hour2   = UTCTime.substring(11,13).toInt();
  int minute1 = weatherTime.substring(14,16).toInt(), minute2 = UTCTime.substring(14,16).toInt();
  int second1 = weatherTime.substring(17,19).toInt(), second2 = UTCTime.substring(17,19).toInt();
  
  if (year1>=year2 && month1>=month2 && day1>=day2 && hour1>=hour2 && minute1>=minute2 && second1>=second2) {
    return 0;
  }
  return 1;
}

void loop() {
  timeClient.update(); // Refreshes NTP Client Connection
  //Serial.println(timeClient.getFormattedTime());
  getUTCDateTime();
  Serial.println(getForecast());
  if(compareDate(getForecast(), getUTCDateTime())) { //If the predicted snowfall time has passed,
    digitalWrite(2, HIGH); // Trigger the nano to move the servo.
    delay(1000);
    digitalWrite(2,LOW);
  }
  delay(300*1000); // Check every 5 minutes
}
