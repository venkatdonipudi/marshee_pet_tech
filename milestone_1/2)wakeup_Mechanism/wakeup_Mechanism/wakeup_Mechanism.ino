/*1.2 Wake-up Mechanism

Implement deep sleep functionality using ESP32's sleep modes

Use ESP32's RTC memory to store wake-up time and counters

Implement wake-on-motion using the simulated accelerometer

Wake up every 2 minutes to update time when inactive*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Structure for IMU data
struct IMUdata {
    float x, y, z;
};

// SimulatedQMI8658 class
class SimulatedQMI8658 {
private:
    bool motionDetected;
    void (*wakeupCallback)();

public:
    SimulatedQMI8658() : motionDetected(false), wakeupCallback(nullptr) {}

    bool begin() {
        Serial.println("Sensor initialized.");
        return true;
    }

    bool readFromFifo(IMUdata* acc, int accCount, IMUdata* gyr, int gyrCount) {
        Serial.println("Reading from FIFO...");
        for (int i = 0; i < accCount; i++) {
            acc[i].x = random(-32768, 32767) / 1000.0;
            acc[i].y = random(-32768, 32767) / 1000.0;
            acc[i].z = random(-32768, 32767) / 1000.0;
        }
        for (int i = 0; i < gyrCount; i++) {
            gyr[i].x = random(-32768, 32767) / 100.0;
            gyr[i].y = random(-32768, 32767) / 100.0;
            gyr[i].z = random(-32768, 32767) / 100.0;
        }
        return true;
    }

    void configWakeOnMotion() {
        Serial.println("Wake-on-motion configured.");
    }

    void setWakeupMotionEventCallBack(void (*callback)()) {
        wakeupCallback = callback;
    }

    void simulateMotion() {
        if (random(100) < 5 && !motionDetected) { // Increase probability for testing
            motionDetected = true;
            if (wakeupCallback) {
                wakeupCallback();
            }
        } else {
            motionDetected = false;
        }
    }

    bool isMotionDetected() const {
        return motionDetected;
    }
};

// Motion detection callback
void onMotionDetected() {
    Serial.println("Motion detected!");
    esp_sleep_enable_timer_wakeup(120 * 1000000); // Set wake-up timer for 2 minutes
    esp_deep_sleep_start(); // Enter deep sleep
}

// Create an instance of the simulated sensor
SimulatedQMI8658 sensor;

bool motionDetectedFlag = false; // Flag to track motion detection

void setup() {
    Serial.begin(115200);
    sensor.begin();
    sensor.setWakeupMotionEventCallBack(onMotionDetected);
    sensor.configWakeOnMotion();

    // Enable deep sleep timer to wake up every 2 minutes 
    esp_sleep_enable_timer_wakeup(10 * 1000000); // 2 minutes in microseconds

    Serial.println("Going to sleep...");
    esp_deep_sleep_start(); // Enter deep sleep
}

void loop() {
  
    IMUdata acc[1];
    IMUdata gyr[1];

    sensor.simulateMotion(); // Simulate and check for motion
    motionDetectedFlag = sensor.isMotionDetected();

    // Only read data if motion was detected
    if (motionDetectedFlag) {
        // Read from the simulated sensor FIFO
        if (sensor.readFromFifo(acc, 1, gyr, 1)) {
            Serial.print("Accelerometer Data - X: "); Serial.print(acc[0].x);
            Serial.print(" Y: "); Serial.print(acc[0].y);
            Serial.print(" Z: "); Serial.println(acc[0].z);

            Serial.print("Gyroscope Data - X: "); Serial.print(gyr[0].x);
            Serial.print(" Y: "); Serial.print(gyr[0].y);
            Serial.print(" Z: "); Serial.println(gyr[0].z);
        }
        
        delay(5000); 
    }

    Serial.println("Going to sleep...");
    esp_deep_sleep_start(); // Go back to deep sleep
}
