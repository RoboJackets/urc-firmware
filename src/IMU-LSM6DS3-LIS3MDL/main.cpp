#include <Arduino.h>
#include <Adafruit_LSM6DS3TRC.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_AHRS.h>
#include <elapsedMillis.h>


// simple vector struct for holding sensor readings
struct vector {

  vector() : x{0}, y{0}, z{0} {}
  vector(double _x, double _y, double _z) : x{_x}, y{_y}, z{_z} {} 

  double x;
  double y;
  double z;
};

// variables
Adafruit_LSM6DS3TRC lsm6ds;
Adafruit_LIS3MDL lis3mdl;
// Adafruit_Madgwick filter;
Adafruit_NXPSensorFusion filter;

vector accel_vec;
vector gyro_vec;
vector mag_vec;
vector from{0, 1, 0};

// timer variables
elapsedMillis filterTimer;
elapsedMillis printTimer;

// constants
static constexpr double DEGREES_PER_RADIAN = (180.0 / PI);
constexpr float FILTER_UPDATE_RATE_HZ = 100;
constexpr float PRINT_TIMER_MS = 500;

const double hard_iron[3] = {
    -38.13, 19.85, -38.23
};

const double soft_iron[3][3] = {
    1.008, 0.082, -0.004,
    0.082, 1.003, 0.019,
    -0.004, 0.019, 0.996
};

void printEulerAngles(double roll, double pitch, double heading);
void vector_cross(const vector &a, const vector &b, vector &out);
float vector_dot(const vector &a, const vector &b);
void vector_normalize(vector &a);

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    bool lsm6ds_success, lis3mdl_success;
    lsm6ds_success = lsm6ds.begin_I2C();
    lis3mdl_success = lis3mdl.begin_I2C();

    if (!lsm6ds_success){
        Serial.println("Failed to find LSM6DS chip");
    }
    if (!lis3mdl_success){
        Serial.println("Failed to find LIS3MDL chip");
    }
    if (!(lsm6ds_success && lis3mdl_success)) {
        while (1) {
        delay(10);
        }
    }

    lis3mdl.setDataRate(LIS3MDL_DATARATE_155_HZ);
    lis3mdl.setRange(LIS3MDL_RANGE_4_GAUSS);
    lis3mdl.setPerformanceMode(LIS3MDL_ULTRAHIGHMODE);
    lis3mdl.setOperationMode(LIS3MDL_CONTINUOUSMODE);
    lis3mdl.setIntThreshold(500);
    lis3mdl.configInterrupt(false, false, true, // enable z axis
                            true, // polarity
                            false, // don't latch
                            true); // enabled!

    filter.begin(FILTER_UPDATE_RATE_HZ);
}

void loop() {
    sensors_event_t accel, gyro, mag, temp;
    
    if (filterTimer >= (1000.0 / FILTER_UPDATE_RATE_HZ)) {
        filterTimer -= (1000.0 / FILTER_UPDATE_RATE_HZ);

        lsm6ds.getEvent(&accel, &gyro, &temp);
        lis3mdl.getEvent(&mag);

        accel_vec = {accel.acceleration.x, accel.acceleration.y, accel.acceleration.z};
        gyro_vec = {gyro.gyro.x * DEGREES_PER_RADIAN, gyro.gyro.y * DEGREES_PER_RADIAN, gyro.gyro.z * DEGREES_PER_RADIAN};
        mag_vec = {mag.magnetic.x, mag.magnetic.y, mag.magnetic.z};

        filter.update(gyro_vec.x, gyro_vec.y, gyro_vec.z, 
                    accel_vec.x, accel_vec.y, gyro_vec.z, 
                    0,0,0);
    }

    if (printTimer >= PRINT_TIMER_MS) {
        printTimer -= PRINT_TIMER_MS;

        double hi_cal[3];
        hi_cal[0] = mag_vec.x - hard_iron[0];
        hi_cal[1] = mag_vec.y - hard_iron[1];
        hi_cal[2] = mag_vec.z - hard_iron[2];

        mag_vec.x = (soft_iron[0][0] * hi_cal[0]) + (soft_iron[0][1] * hi_cal[1]) + (soft_iron[0][2] * hi_cal[2]);
        mag_vec.y = (soft_iron[1][0] * hi_cal[0]) + (soft_iron[1][1] * hi_cal[1]) + (soft_iron[1][2] * hi_cal[2]);
        mag_vec.z = (soft_iron[2][0] * hi_cal[0]) + (soft_iron[2][1] * hi_cal[1]) + (soft_iron[2][2] * hi_cal[2]);

        vector E;
        vector N;
        vector_cross(mag_vec, accel_vec, E);
        vector_normalize(E);
        vector_cross(accel_vec, E, N);
        vector_normalize(N);

        double heading = atan2f(vector_dot(E, from), vector_dot(N, from)) * DEGREES_PER_RADIAN;
        if (heading < 0) heading += 360;

        printEulerAngles(filter.getRoll(), filter.getPitch(), heading);
    }
}

void printEulerAngles(double roll, double pitch, double heading) {
    Serial.print("[roll=");
    Serial.print(roll);
    Serial.print(",pitch=");
    Serial.print(pitch);
    Serial.print(",heading=");
    Serial.print(heading);
    Serial.println("]");
}

void vector_cross(const vector &a, const vector &b, vector &out) {
    out.x = (a.y * b.z) - (a.z * b.y);
    out.y = (a.z * b.x) - (a.x * b.z);
    out.z = (a.x * b.y) - (a.y * b.x);
}

float vector_dot(const vector &a, const vector &b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

void vector_normalize(vector &a) {
    double magnitude = sqrt(vector_dot(a, a));
    a.x /= magnitude;
    a.y /= magnitude;
    a.z /= magnitude;
}