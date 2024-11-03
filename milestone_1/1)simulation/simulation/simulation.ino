/*1.1 Accelerometer Simulation

Create a dummy accelerometer module that simulates the QMI8658 behavior

Implement the following functions:

begin(): Initialize the simulated sensor

readFromFifo(IMUdata* acc, int accCount, IMUdata* gyr, int gyrCount): Simulate reading from FIFO

configWakeOnMotion(): Configure wake-on-motion settings

setWakeupMotionEventCallBack(callback): Set callback for motion events*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// Structure for IMU data
struct IMUdata {
    float x, y, z;
};

// SimulatedQMI8658 class
class SimulatedQMI8658 {
private:
    bool motionDetected;
    void (*wakeupCallback)();  // Callback function pointer

public:
    SimulatedQMI8658() : motionDetected(false), wakeupCallback(nullptr) {}

    bool begin() {
        Serial.println("Sensor initialized.");
        return true;
    }

    bool readFromFifo(IMUdata* acc, int accCount, IMUdata* gyr, int gyrCount) {
        Serial.println("Reading from FIFO...");
        for (int i = 0; i < accCount; i++) {
            // Changed accelerometer ranges
            acc[i].x = random(-16000, 16000) / 2000.0;
            acc[i].y = random(-16000, 16000) / 2000.0;
            acc[i].z = random(-16000, 16000) / 2000.0;
        }
        for (int i = 0; i < gyrCount; i++) {
            // Changed gyroscope ranges
            gyr[i].x = random(-8000, 8000) / 150.0;
            gyr[i].y = random(-8000, 8000) / 150.0;
            gyr[i].z = random(-8000, 8000) / 150.0;
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

// Motion detection callback function
void onMotionDetected() {
    Serial.println("Motion detected!");
}

// Create an instance of the simulated sensor
SimulatedQMI8658 sensor;

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
        Serial.print("Accelerometer Data - X: "); Serial.print(acc[0].x);
        Serial.print(" Y: "); Serial.print(acc[0].y);
        Serial.print(" Z: "); Serial.println(acc[0].z);

        Serial.print("Gyroscope Data - X: "); Serial.print(gyr[0].x);
        Serial.print(" Y: "); Serial.print(gyr[0].y);
        Serial.print(" Z: "); Serial.println(gyr[0].z);
    }

    delay(1000);
}
