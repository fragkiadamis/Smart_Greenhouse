// BluetoothCom.h
#include "Arduino.h"

#ifndef SMART_GREENHOUSE_SERIAL_H
#define SMART_GREENHOUSE_SERIAL_H

// Serials
#define DEBUG // Comment it out for production, serial prints waste flash and dynamic memory...
#define SERIAL_BUFFER_SIZE 16
#define BAUD_RATE 115200
#define BT_BAUD_RATE 38400
#define SLAVE_BT_RX 10
#define SLAVE_BT_TX 11
#define MASTER_BT_RX 16
#define MASTER_BT_TX 17

class SmartGreenHouseSerial {
    private:
    public:
        // DHT sensors enumeration - Communication Protocol
        typedef enum {
            HUMIDITY,
            INNER_TEMP
        } DHT_SENSOR;

        // Action enumeration - Communication Protocol
        typedef enum {
            BZ,         // Buzzer
            DHT_SENS,   // DHT Humidity
            IRG,        // Irrigation
            OUTER_TEMP, // Outer temperature
            LUM         // Inner lumination
        } ACTION;

        // Methods
        void setupHardwareSerial(void);
        void setupBTSerial(void);
        bool hasMessage(void);
        String receive(void);
        void send(String msg);
};

#endif