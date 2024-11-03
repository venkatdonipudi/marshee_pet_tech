/*1.4 Basic Activity Classification

Implement a simple activity classifier using predefined thresholds

Classify activities into Resting, Walking, Running, and Playing*/
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 
#include <math.h>

// Define IMUdata structure
struct IMUdata {
    float x, y, z;
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
        return true; }

    bool readFromFifo(IMUdata* acc, int accCount, IMUdata* gyr, int gyrCount) {
        Serial.println("Reading from FIFO...");
        for (int i = 0; i < accCount; i++) {
            // Updated accelerometer 
            acc[i].x = random(-16000, 16000) / 2000.0;
            acc[i].y = random(-16000, 16000) / 2000.0;
            acc[i].z = random(-16000, 16000) / 2000.0;
        }
        for (int i = 0; i < gyrCount; i++) {
            // Updated gyroscope ranges
            gyr[i].x = random(-8000, 8000) / 150.0;
            gyr[i].y = random(-8000, 8000) / 150.0;
            gyr[i].z = random(-8000, 8000) / 150.0; }
        return true; }

    void configWakeOnMotion() {
        Serial.println("Wake-on-motion configured."); }
   
    void setWakeupMotionEventCallBack(void (*callback)()) {
        wakeupCallback = callback; }
   
    void simulateMotion() {
        if (random(100) < 5 && !motionDetected) {
            motionDetected = true;
            if (wakeupCallback) {
                wakeupCallback();
            }
        } else {
            motionDetected = false; }
      
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
    const float restingThreshold = 0.5;  // Threshold for resting
    const float walkingThreshold = 2.0;  // Threshold for walking
    const float runningThreshold = 5.0;  // Threshold for running
};

// Create an instance of the classifier
ActivityClassifier activityClassifier;

void setup() {
    Serial.begin(115200);
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
    }

    delay(1000);
}
