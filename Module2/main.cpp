#include "mbed.h"
#include "MMA8451Q.h"
#include "TSISensor.h"
PinName const SDA = PTE25;
PinName const SCL = PTE24;
#define MMA8451_I2C_ADDRESS (0x1d<<1)
#define AXIS_DIFF           0.75

Serial pc(USBTX, USBRX);

int main(void)
{
    MMA8451Q acc(SDA, SCL, MMA8451_I2C_ADDRESS);
    PwmOut rled(LED1);
    PwmOut gled(LED2);
    PwmOut bled(LED3);
    TSISensor tsi;
    Timer t;
    
    rled = 1.00f;
    gled = 1.00f;
    bled = 1.00f;
    int LEDLast = 0;
    float lastRead = 0.5;
    
    while (true) {
        t.start();
        float x, y, z;
        x = abs(acc.getAccX());
        y = abs(acc.getAccY());
        z = abs(acc.getAccZ());  //Account for z-axis gravity bias
        
        /*Turn on the LED which axis has the greatest acceleration*/
        if((abs(x - y) > AXIS_DIFF) && (abs(x - z) > AXIS_DIFF)){
            rled = 1.00f - x + lastRead;
            gled = 1.00f;
            bled = 1.00f;
            LEDLast = 1;
            
        }
        else if((abs(y - z) > AXIS_DIFF) && (abs(y - x) > AXIS_DIFF)){
            gled = 1.00f - y + lastRead;
            rled = 1.00f;
            bled = 1.00f;
            LEDLast = 2;
        }
        else if((abs(z - y) > AXIS_DIFF) && (abs(z - x) > AXIS_DIFF)){
            bled = 1.00f - z + lastRead;
            rled = 1.00f;
            gled = 1.00f;
            LEDLast = 3;
        }
        
        /*If the device is not in motion, keep the last result*/
        else{
            switch(LEDLast){
                case 1:
                    rled = 1.000f - x + lastRead;
                    gled = 1.00f;
                    bled = 1.00f;
                    break;
                case 2:
                    rled = 1.00f;
                    gled = 1.000f - y + lastRead;
                    bled = 1.00f;
                    break;
                case 3:
                    rled = 1.00f;
                    gled = 1.00f;
                    bled = 1.000f - z + lastRead;
                    break;
                default:
                    break;
                }
        }        
        float newRead = tsi.readPercentage();
        if (newRead != 0)
            lastRead = newRead;
                    
        t.stop();
        pc.printf("The time taken was %f seconds\n", t.read());
        t.reset();
        
        wait(0.1);
    }
}
