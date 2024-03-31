#include <Adafruit_LSM6DS3TRC.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_AHRS.h>
#include <elapsedMillis.h>

// Sensor: Adafruit LSM6DS3 + LIS3MDL
Adafruit_LSM6DS3TRC lsm6ds;
Adafruit_LIS3MDL lis3mdl;

// constants
constexpr double DEGREES_PER_RADIAN = (180.0 / PI);

// const double hard_iron[3] = {
//     -38.13, 19.85, -38.23
// };

// const double soft_iron[3][3] = {
//     1.008, 0.082, -0.004,
//     0.082, 1.003, 0.019,
//     -0.004, 0.019, 0.996
// };

const double hard_iron[3] = {
    0,0,0
};

const double soft_iron[3][3] = {
    1,0,0,
    0,1,0,
    0,0,1
};



// functions
void setAccelDataRange();
void setAccelDataRate();
void setGyroDataRate();
void setMagDataRate();
void setMagDataRange();
void setMagPerformance();
void setMagOperationMode();

void setup(void) {
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

    Serial.println("LSM6DS and LIS3MDL Found!");

    setAccelDataRange();
    setAccelDataRate();
    setGyroDataRate();
    setMagDataRate();
    setMagDataRange();
    setMagPerformance();
    setMagOperationMode();
    
    lis3mdl.setIntThreshold(500);
    lis3mdl.configInterrupt(false, false, true, // enable z axis
                            true, // polarity
                            false, // don't latch
                            true); // enabled!
}

void loop() {
    sensors_event_t accel, gyro, mag, temp;

    // read IMU data
    lsm6ds.getEvent(&accel, &gyro, &temp);
    lis3mdl.getEvent(&mag);

    double hi_cal[3];
    double mag_data[3] = { mag.magnetic.x, mag.magnetic.y, mag.magnetic.z };

    // apply hard iron calibration
    for (int i = 0; i < 3; i++) {
        hi_cal[i] = mag_data[i] - hard_iron[i];
    }

    // apply soft iron calibration
    for (int i = 0; i < 3; i++) {
        mag_data[i] = (soft_iron[i][0] * hi_cal[0] + 
                        soft_iron[i][1] * hi_cal[1] +
                        soft_iron[i][2] * hi_cal[2]);
    }

    // 'Raw' values to match expectation of MotionCal
    Serial.print("Raw:");
    Serial.print(int(accel.acceleration.x*8192/9.8)); Serial.print(",");
    Serial.print(int(accel.acceleration.y*8192/9.8)); Serial.print(",");
    Serial.print(int(accel.acceleration.z*8192/9.8)); Serial.print(",");
    Serial.print(int(gyro.gyro.x*DEGREES_PER_RADIAN*16)); Serial.print(",");
    Serial.print(int(gyro.gyro.y*DEGREES_PER_RADIAN*16)); Serial.print(",");
    Serial.print(int(gyro.gyro.z*DEGREES_PER_RADIAN*16)); Serial.print(",");
    Serial.print(int(mag_data[0]*10)); Serial.print(",");
    Serial.print(int(mag_data[1]*10)); Serial.print(",");
    Serial.print(int(mag_data[2]*10)); Serial.println("");
 
    // IMPORTANT!
    // Do NOT change this delay!
    // MotionCal cannot handle data coming in super quickly!
    delay(10);
}

void setAccelDataRange() {
     // lsm6ds.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
    Serial.print("Accelerometer range set to: ");
    switch (lsm6ds.getAccelRange()) {
    case LSM6DS_ACCEL_RANGE_2_G:
        Serial.println("+-2G");
        break;
    case LSM6DS_ACCEL_RANGE_4_G:
        Serial.println("+-4G");
        break;
    case LSM6DS_ACCEL_RANGE_8_G:
        Serial.println("+-8G");
        break;
    case LSM6DS_ACCEL_RANGE_16_G:
        Serial.println("+-16G");
        break;
    }
}

void setAccelDataRate() {
    // lsm6ds.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
    Serial.print("Accelerometer data rate set to: ");
    switch (lsm6ds.getAccelDataRate()) {
        case LSM6DS_RATE_SHUTDOWN:
        Serial.println("0 Hz");
        break;
    case LSM6DS_RATE_12_5_HZ:
        Serial.println("12.5 Hz");
        break;
    case LSM6DS_RATE_26_HZ:
        Serial.println("26 Hz");
        break;
    case LSM6DS_RATE_52_HZ:
        Serial.println("52 Hz");
        break;
    case LSM6DS_RATE_104_HZ:
        Serial.println("104 Hz");
        break;
    case LSM6DS_RATE_208_HZ:
        Serial.println("208 Hz");
        break;
    case LSM6DS_RATE_416_HZ:
        Serial.println("416 Hz");
        break;
    case LSM6DS_RATE_833_HZ:
        Serial.println("833 Hz");
        break;
    case LSM6DS_RATE_1_66K_HZ:
        Serial.println("1.66 KHz");
        break;
    case LSM6DS_RATE_3_33K_HZ:
        Serial.println("3.33 KHz");
        break;
    case LSM6DS_RATE_6_66K_HZ:
        Serial.println("6.66 KHz");
        break;
    }
}

void setGyroDataRate() {
    // lsm6ds.setGyroDataRate(LSM6DS_RATE_12_5_HZ);
    Serial.print("Gyro data rate set to: ");
    switch (lsm6ds.getGyroDataRate()) {
        case LSM6DS_RATE_SHUTDOWN:
        Serial.println("0 Hz");
        break;
    case LSM6DS_RATE_12_5_HZ:
        Serial.println("12.5 Hz");
        break;
    case LSM6DS_RATE_26_HZ:
        Serial.println("26 Hz");
        break;
    case LSM6DS_RATE_52_HZ:
        Serial.println("52 Hz");
        break;
    case LSM6DS_RATE_104_HZ:
        Serial.println("104 Hz");
        break;
    case LSM6DS_RATE_208_HZ:
        Serial.println("208 Hz");
        break;
    case LSM6DS_RATE_416_HZ:
        Serial.println("416 Hz");
        break;
    case LSM6DS_RATE_833_HZ:
        Serial.println("833 Hz");
        break;
    case LSM6DS_RATE_1_66K_HZ:
        Serial.println("1.66 KHz");
        break;
    case LSM6DS_RATE_3_33K_HZ:
        Serial.println("3.33 KHz");
        break;
    case LSM6DS_RATE_6_66K_HZ:
        Serial.println("6.66 KHz");
        break;
    }
}

void setMagDataRate() {
    lis3mdl.setDataRate(LIS3MDL_DATARATE_155_HZ);
    // You can check the datarate by looking at the frequency of the DRDY pin
    Serial.print("Magnetometer data rate set to: ");
    switch (lis3mdl.getDataRate()) {
        case LIS3MDL_DATARATE_0_625_HZ: Serial.println("0.625 Hz"); break;
        case LIS3MDL_DATARATE_1_25_HZ: Serial.println("1.25 Hz"); break;
        case LIS3MDL_DATARATE_2_5_HZ: Serial.println("2.5 Hz"); break;
        case LIS3MDL_DATARATE_5_HZ: Serial.println("5 Hz"); break;
        case LIS3MDL_DATARATE_10_HZ: Serial.println("10 Hz"); break;
        case LIS3MDL_DATARATE_20_HZ: Serial.println("20 Hz"); break;
        case LIS3MDL_DATARATE_40_HZ: Serial.println("40 Hz"); break;
        case LIS3MDL_DATARATE_80_HZ: Serial.println("80 Hz"); break;
        case LIS3MDL_DATARATE_155_HZ: Serial.println("155 Hz"); break;
        case LIS3MDL_DATARATE_300_HZ: Serial.println("300 Hz"); break;
        case LIS3MDL_DATARATE_560_HZ: Serial.println("560 Hz"); break;
        case LIS3MDL_DATARATE_1000_HZ: Serial.println("1000 Hz"); break;
    }
}

void setMagDataRange() {
    lis3mdl.setRange(LIS3MDL_RANGE_4_GAUSS);
    Serial.print("Range set to: ");
    switch (lis3mdl.getRange()) {
        case LIS3MDL_RANGE_4_GAUSS: Serial.println("+-4 gauss"); break;
        case LIS3MDL_RANGE_8_GAUSS: Serial.println("+-8 gauss"); break;
        case LIS3MDL_RANGE_12_GAUSS: Serial.println("+-12 gauss"); break;
        case LIS3MDL_RANGE_16_GAUSS: Serial.println("+-16 gauss"); break;
    }
}

void setMagPerformance() {
    lis3mdl.setPerformanceMode(LIS3MDL_ULTRAHIGHMODE);
    Serial.print("Magnetometer performance mode set to: ");
    switch (lis3mdl.getPerformanceMode()) {
        case LIS3MDL_LOWPOWERMODE: Serial.println("Low"); break;
        case LIS3MDL_MEDIUMMODE: Serial.println("Medium"); break;
        case LIS3MDL_HIGHMODE: Serial.println("High"); break;
        case LIS3MDL_ULTRAHIGHMODE: Serial.println("Ultra-High"); break;
    }
}

void setMagOperationMode() {
    lis3mdl.setOperationMode(LIS3MDL_CONTINUOUSMODE);
    Serial.print("Magnetometer operation mode set to: ");
    // Single shot mode will complete conversion and go into power down
    switch (lis3mdl.getOperationMode()) {
        case LIS3MDL_CONTINUOUSMODE: Serial.println("Continuous"); break;
        case LIS3MDL_SINGLEMODE: Serial.println("Single mode"); break;
        case LIS3MDL_POWERDOWNMODE: Serial.println("Power-down"); break;
    }
}