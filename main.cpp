#include <pico/stdlib.h>
#include "hardware/i2c.h"

#define sda_pin 12
#define scl_pin 13

#define addr 0x60

#define writeToDACRegisterMode 8
#define writeToDACEEPROMMode 18

#define DACMaxValue 495

enum WriteMode {
    Register,
    Eeprom
};

int writeToDAC(int loadPercentage, WriteMode mode = Register) {
    if (loadPercentage > 100 || loadPercentage < 0)
        return -2;
    int percent = loadPercentage/100;
    int load = percent * DACMaxValue;

    uint8_t data[3];
    data[0] = writeToDACRegisterMode;
/*     if (mode == Register) {
        data[0] = writeToDACRegisterMode;
    }
    else {
        data[0] = writeToDACEEPROMMode;
    } */

    if (load > 255) {
        data[1] = 255;
        int value = load - 255;
        data[2] = value;
    }
    else {
        data[1] = load;
        data[2] = 0;
    }
    return i2c_write_blocking(i2c0, addr, data, 3, false);
}


int main() {
    i2c_init(i2c_default, 100 * 1000);
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    gpio_put(25, 1);

    int eh = writeToDAC(80);
    if (eh == PICO_ERROR_GENERIC) {
        while(true) {
            //error connecting to DAC
            gpio_put(25, 0);
            sleep_ms(500);
            gpio_put(25, 1);
            sleep_ms(500);
        }
    }

    int test = 100;
    while (true) {
        test--;
        if (test <= 0)
            test = 100;
        writeToDAC(test);
        sleep_ms(100); 
    }
    int stepDown = 255;
    int stepDown2 = DACMaxValue - 255;
    uint8_t data[2];
    data[0] = writeToDACRegisterMode;
    while (true) {
        stepDown--;
        stepDown2--;
        if (stepDown <= 0) {
            stepDown = 255;
        }
        if (stepDown2 <= 0) {
            stepDown2 = 240;
        }
        data[1] = stepDown;
        data[2] = stepDown2;
        i2c_write_blocking(i2c0, addr, data, 3, false);
        sleep_ms(10); 

    }
    return 0;
}