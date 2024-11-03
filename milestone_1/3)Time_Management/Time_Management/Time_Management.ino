/*1.3 Time Management

Implement a TimeManager class to handle time-related functions

Use ESP32's RTC for timekeeping

Implement NTP synchronization when Wi-Fi is available*/
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <WiFi.h>
#include <time.h>

class TimeManager {
public:
    void begin() {
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    }

    void syncTime() {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Syncing time with NTP server...");
            begin();
            struct tm timeinfo;
            if (!getLocalTime(&timeinfo)) {
                Serial.println("Failed to obtain time");
                return;
            }
            Serial.println("Time synchronized successfully");
            Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");}
         else {
            Serial.println("WiFi not connected; cannot sync time.");}      
    }

    String getTime() {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            return "Failed to obtain time"; }      
        char timeString[20];
        strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeinfo);
        return String(timeString); }

private:
    const char* ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = 19800;         // for Indian Standard Time (IST)
    const int daylightOffset_sec = 0;         // No daylight savings in India



// Create an instance of TimeManager
TimeManager timeManager;

void setup() {
    Serial.begin(115200);

    // Connect to Wi-Fi
    WiFi.begin("vampires", "Vampire@2003");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Sync time with NTP
    timeManager.syncTime();
}

void loop() {
    // Print current time
    Serial.println("Current Time: " + timeManager.getTime());
    delay(10000); // Check time every 10 seconds for testing
}