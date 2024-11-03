/*1.5 Data Storage Structure

Design and implement an efficient data storage structure using LittleFS

Store activity data in 10-minute intervals

Data format: timestamp, activity durations (in seconds) for each activity type

Example LittleFS data structure:

/data/

  ├── YYYYMMDD.dat

  ├── YYYYMMDD.dat

  └── ...
File format (binary):
[timestamp: uint32][activity_0: uint16][activity_1: uint16][activity_2: uint16]...*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#include <math.h>
#include <LittleFS.h>

// Define IMUdata structure
struct IMUdata {
    float x, y, z;
};

// Define Activity Data Structure
struct ActivityData {
    uint32_t timestamp;                    // Timestamp for the 10-minute interval
    uint16_t activityDurations[4];         // Array for activity durations 
};

// SimulatedQMI8658 
class SimulatedQMI8658 {
private:
    bool motionDetected;
    void (*wakeupCallback)();  // Callback function 

public:
    SimulatedQMI8658() : motionDetected(false), wakeupCallback(nullptr) {}

    bool begin() {
        Serial.println("Sensor initialized.");
        return true;
    }

    bool readFromFifo(IMUdata* acc, int accCount, IMUdata* gyr, int gyrCount) {
        Serial.println("Reading from FIFO...");
        for (int i = 0; i < accCount; i++) {
            acc[i].x = random(-16000, 16000) / 2000.0;
            acc[i].y = random(-16000, 16000) / 2000.0;
            acc[i].z = random(-16000, 16000) / 2000.0;
        }
        for (int i = 0; i < gyrCount; i++) {
            gyr[i].x = random(-8000, 8000) / 150.0;
            gyr[i].y = random(-8000, 8000) / 150.0;
            gyr[i].z = random(-8000, 8000) / 150.0;}
        
        return true;
    }

    void configWakeOnMotion() {
        Serial.println("Wake-on-motion configured.");}
    

    void setWakeupMotionEventCallBack(void (*callback)()) {
        wakeupCallback = callback;}

    void simulateMotion() {
        if (random(100) < 5 && !motionDetected) {
            motionDetected = true;
            if (wakeupCallback) {
                wakeupCallback();
            }
        } else {
            motionDetected = false;
        }
    }
};

// Motion detection 
void onMotionDetected() {
    Serial.println("Motion detected!");
}

// Create an instance of the simulated sensor
SimulatedQMI8658 sensor;

class ActivityClassifier {
public:
    String classifyActivity(float ax, float ay, float az) {
        float totalAcc = sqrt(ax * ax + ay * ay + az * az);

        if (totalAcc < restingThreshold) {
            return "Resting";
        } else if (totalAcc < walkingThreshold) {
            return "Walking";
        } else if (totalAcc < runningThreshold) {
            return "Running";
        } else {
            return "Playing";
        }
    }

private:
    const float restingThreshold = 0.5;  //  for resting
    const float walkingThreshold = 2.0;  //  for walking
    const float runningThreshold = 5.0;  //  for running
};

// Create an instance of the classifier
ActivityClassifier activityClassifier;

// Initialize variables for storing activity data
ActivityData currentActivityData;
uint32_t lastTimestamp = 0;  // Keep track of the last timestamp for 10-minute intervals
const uint32_t INTERVAL_SECONDS = 600;  // 10 minutes

void setup() {
    Serial.begin(115200);
    
    if (!LittleFS.begin()) {
        Serial.println("LittleFS mount failed");
        return;
    }

    sensor.begin();
    Serial.println("Process begin");
    sensor.setWakeupMotionEventCallBack(onMotionDetected);
    sensor.configWakeOnMotion();
}

void loop() {
    IMUdata acc[1];
    IMUdata gyr[1];

    // Simulate motion detection
    sensor.simulateMotion();

    // Read from the simulated sensor FIFO
    if (sensor.readFromFifo(acc, 1, gyr, 1)) {
        // Classify activity based on accelerometer data
        String activity = activityClassifier.classifyActivity(acc[0].x, acc[0].y, acc[0].z);
        Serial.print("Current Activity: ");
        Serial.println(activity);

        // Print accelerometer data
        Serial.print("Accelerometer Data - X: "); Serial.print(acc[0].x);
        Serial.print(" Y: "); Serial.print(acc[0].y);
        Serial.print(" Z: "); Serial.println(acc[0].z);

        // Print gyroscope data
        Serial.print("Gyroscope Data - X: "); Serial.print(gyr[0].x);
        Serial.print(" Y: "); Serial.print(gyr[0].y);
        Serial.print(" Z: "); Serial.println(gyr[0].z);

        // Store activity duration in the current interval
        storeActivityData(activity);
    }

    // Check if 10 minutes have passed
    uint32_t currentMillis = millis();
    if (currentMillis - lastTimestamp >= INTERVAL_SECONDS * 1000) {
        saveActivityDataToFile();
        lastTimestamp = currentMillis;  // Update last timestamp
    }

    delay(1000); // Adjust delay for testing frequency
}

void storeActivityData(const String& activity) {
    // Increment activity duration based on the current activity
    if (activity == "Resting") {
        currentActivityData.activityDurations[0]++;
    } else if (activity == "Walking") {
        currentActivityData.activityDurations[1]++;
    } else if (activity == "Running") {
        currentActivityData.activityDurations[2]++;
    } else if (activity == "Playing") {
        currentActivityData.activityDurations[3]++;
    }
}

void saveActivityDataToFile() {
    // Get current timestamp
    currentActivityData.timestamp = millis() / 1000;  // Convert milliseconds to seconds

    // Generate filename based on the current date
    char filename[20];
    generateFilename(currentActivityData.timestamp, filename);

    // Create or open the file for appending in binary mode
    File dataFile = LittleFS.open(filename, "a");

    if (!dataFile) {
        Serial.println("Failed to open file for writing");
        return;
    }

    //  file in binary format
    dataFile.write((uint8_t*)&currentActivityData, sizeof(currentActivityData));
    dataFile.close();

    Serial.println("Activity data successfully written to file.");

    // Reset current activity data
    memset(&currentActivityData.activityDurations, 0, sizeof(currentActivityData.activityDurations));
}

void generateFilename(uint32_t timestamp, char* filename) {
    struct tm timeInfo;
    if (!gmtime_r((time_t*)&timestamp, &timeInfo)) {
        Serial.println("Failed to convert timestamp to date.");
        return;
    }
    snprintf(filename, 20, "/data/%04d%02d%02d.dat",
             timeInfo.tm_year + 1900,
             timeInfo.tm_mon + 1,
             timeInfo.tm_mday);
}
