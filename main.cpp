#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico.h"
#include <cstdlib>

#define sda_pin 12
#define scl_pin 13

//1100(default addr) + 
//00(A2, A1 Defaulted to 0) + 
//0(A0 user selectable, Soldered A0 to ground in this case)
#define addr 0x60
//010 (c2 = 0, c1 = 1, c0 = 0)
//00 (powerdown mode = normal operation)
#define writeNormalMode 8
//011 (c2 = 0, c1 = 1, c0 = 1)
//00 (powerdown mode = normal operation)
#define writeEEPROMMode 18
//12 bit register, 8 = 11111111(first byte, max value = 255)
//4 = 11110000(second byte, max value = 240)
//Total max value = 495;
#define DACMaxValue 495

enum WriteMode {
    FastMode,
    NormalMode,
    EepromMode
};

uint8_t * formatData(int load) {
    uint8_t * ptr = (uint8_t*)malloc(2);
    if (load > 255) {
        ptr[0] = 255;
        int value = load - 255;
        ptr[1] = value;
    }
    else {
        ptr[0] = load;
        ptr[1] = 0;
    }
    return ptr;
}

int writeToDAC(int loadPercentage, WriteMode mode = FastMode) {
    if (loadPercentage > 100 || loadPercentage < 0)
        return -2;
    double percent = loadPercentage/100.0;
    int load = percent * DACMaxValue;

    uint8_t *ptr = formatData(load);
    uint8_t data[3];

    switch (mode) {
        case FastMode:
            data[0] = ptr[1] >> 4;
            data[1] = ptr[0];
            break;
        case NormalMode:
            data[0] = writeNormalMode;
            data[1] = ptr[0];
            data[2] = ptr[1];
            break;
        case EepromMode:
            data[0] = writeEEPROMMode;
            data[1] = ptr[0];
            data[2] = ptr[1];
            break;
    }
    int result = i2c_write_blocking(i2c0, addr, data, mode == FastMode ? 2 : 3, false);
    free(ptr);
    return result;
}

 
int main() {
    i2c_init(i2c_default, 400000);
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
    bool dir = true;
    while (true) {

        if (dir) {
            test--;
            if (test <= 0)
            {
                dir  = false;
                continue;
            }
        }
        else {
            test++;
            if (test >= 100)
            {
                sleep_ms(500); 
                dir = true;
                continue;
            }
        }
        writeToDAC(test);
        sleep_ms(5); 
    }
    return 0;
}