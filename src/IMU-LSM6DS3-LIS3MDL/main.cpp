#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
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
Adafruit_NXPSensorFusion filter;
Adafruit_NeoPixel status(1, PIN_NEOPIXEL);
bool statusEnabled;

double mag_data[3];
double accel_data[3];
double gyro_data[3];

// timer variables
elapsedMillis filterTimer;
elapsedMillis printTimer;
elapsedMillis blinkTimer;

// constants
static constexpr double DEGREES_PER_RADIAN = (180.0 / PI);
static constexpr double RADIANS_PER_DEGREE = (PI / 180.0);
constexpr float FILTER_UPDATE_RATE_HZ = 100;
constexpr float PRINT_TIMER_MS = 100;
constexpr int BLINK_TIMER_MS = 500;


// calibration constants
const double hard_iron[3] = {
    46.89, 108.58, 1.81
};

const double soft_iron[3][3] = {
    0.941, 0.038, 0.029,
    0.038, 1.004, 0.018,
    0.029, 0.018, 1.061
};

const double declination = -5.517;

// functions
void printEulerAngles(double roll, double pitch, double heading);
void printQuaternion(double roll, double pitch, double heading);
void printAccel(double x, double y, double z);
void vector_cross(const vector &a, const vector &b, vector &out);
float vector_dot(const vector &a, const vector &b);
void vector_normalize(vector &a);

void setup() {
    Serial.begin(115200);

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

    status.begin();
    statusEnabled = false;
}

void loop() {
    sensors_event_t accel, gyro, mag, temp;
    
    if (filterTimer >= (1000.0 / FILTER_UPDATE_RATE_HZ)) {
        filterTimer -= (1000.0 / FILTER_UPDATE_RATE_HZ);

        lsm6ds.getEvent(&accel, &gyro, &temp);
        lis3mdl.getEvent(&mag);

        // ACCEL
        accel_data[0] = accel.acceleration.x;
        accel_data[1] = accel.acceleration.y;
        accel_data[2] = accel.acceleration.z;

        // GYRO
        gyro_data[0] = gyro.gyro.x;
        gyro_data[1] = gyro.gyro.y;
        gyro_data[2] = gyro.gyro.z;

        // translate gyro data to degrees from radians
        for (int i = 0; i < 3; i++) {
            gyro_data[i] *= DEGREES_PER_RADIAN;
        }

        // calculate roll and pitch
        filter.update(gyro_data[0], gyro_data[1], gyro_data[2], 
                    accel_data[0], accel_data[1], accel_data[2], 
                    0,0,0);

        // MAGNETOMETER
        mag_data[0] = mag.magnetic.x; 
        mag_data[1] = mag.magnetic.y; 
        mag_data[2] = mag.magnetic.z; 
    }

    if (printTimer >= PRINT_TIMER_MS) {
        printTimer -= PRINT_TIMER_MS;

        vector accel_vec = {accel.acceleration.x, accel.acceleration.y, accel.acceleration.z};
        vector mag_vec;
        vector from{0, 1, 0};

        // hard iron offset
        double hi_cal[3];
        hi_cal[0] = mag_data[0] - hard_iron[0];
        hi_cal[1] = mag_data[1] - hard_iron[1];
        hi_cal[2] = mag_data[2]  - hard_iron[2];

        // soft iron offset
        mag_vec.x = (soft_iron[0][0] * hi_cal[0]) + (soft_iron[0][1] * hi_cal[1]) + (soft_iron[0][2] * hi_cal[2]);
        mag_vec.y = (soft_iron[1][0] * hi_cal[0]) + (soft_iron[1][1] * hi_cal[1]) + (soft_iron[1][2] * hi_cal[2]);
        mag_vec.z = (soft_iron[2][0] * hi_cal[0]) + (soft_iron[2][1] * hi_cal[1]) + (soft_iron[2][2] * hi_cal[2]);

        // tilt compensation
        vector E;
        vector N;
        vector_cross(mag_vec, accel_vec, E);
        vector_normalize(E);
        vector_cross(accel_vec, E, N);
        vector_normalize(N);

        // calculate heading, apply declination
        double heading = atan2f(vector_dot(E, from), vector_dot(N, from)) * DEGREES_PER_RADIAN;
        if (heading < 0) heading += 360;
        heading += declination;

        // printEulerAngles(filter.getRoll(), filter.getPitch(), heading);
        printQuaternion(filter.getRoll(), filter.getPitch(), heading);
        printAccel(accel_data[0], accel_data[1], accel_data[2]);
    }

    if (blinkTimer >= BLINK_TIMER_MS) {
        blinkTimer -= BLINK_TIMER_MS;

        if (!statusEnabled) {
            status.setPixelColor(0, status.Color(255, 0, 0));
            status.show();
            statusEnabled = true;
        } else {
            status.clear();
            status.show();
            statusEnabled = false;
        }
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

void printAccel(double x, double y, double z) {
    Serial.print("[ax=");
    Serial.print(x);
    Serial.print(", ay=");
    Serial.print(y);
    Serial.print(", az=");
    Serial.print(z);
    Serial.println("]");
}

//     qx = np.sin(roll/2) * np.cos(pitch/2) * np.cos(yaw/2) - np.cos(roll/2) * np.sin(pitch/2) * np.sin(yaw/2)
//     qy = np.cos(roll/2) * np.sin(pitch/2) * np.cos(yaw/2) + np.sin(roll/2) * np.cos(pitch/2) * np.sin(yaw/2)
//     qz = np.cos(roll/2) * np.cos(pitch/2) * np.sin(yaw/2) - np.sin(roll/2) * np.sin(pitch/2) * np.cos(yaw/2)
//     qw = np.cos(roll/2) * np.cos(pitch/2) * np.cos(yaw/2) + np.sin(roll/2) * np.sin(pitch/2) * np.sin(yaw/2)


// positive X axis = true north
// positive Y axis = true east
// positive Z axis = straight up
void printQuaternion(double roll, double pitch, double heading) {
    double roll_arg = (roll * RADIANS_PER_DEGREE) / 2.0;
    double pitch_arg = (pitch * RADIANS_PER_DEGREE) / 2.0;
    double yaw_arg = ((heading) * RADIANS_PER_DEGREE) / 2.0;

    double roll_sin = sinf(roll_arg);
    double roll_cos = cosf(roll_arg);
    double pitch_sin = sinf(pitch_arg);
    double pitch_cos = cosf(pitch_arg);
    double yaw_sin = sinf(yaw_arg);
    double yaw_cos = cosf(yaw_arg);

    double qx = (roll_sin * pitch_cos * yaw_cos) - (roll_cos * pitch_sin * yaw_sin);
    double qy = (roll_cos * pitch_sin * yaw_cos) + (roll_sin * pitch_cos * yaw_sin);
    double qz = (roll_cos * pitch_cos * yaw_sin) - (roll_sin * pitch_sin * yaw_cos);
    double qw = (roll_cos * pitch_cos * yaw_cos) + (roll_sin * pitch_sin * yaw_sin);

    Serial.print("[qx=");
    Serial.print(qx);
    Serial.print(",qy=");
    Serial.print(qy);
    Serial.print(",qz=");
    Serial.print(qz);
    Serial.print(",qw=");
    Serial.print(qw);
    Serial.print(", yaw=");
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
