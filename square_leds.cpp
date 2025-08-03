#include <Arduino.h>
#include <IRremote.hpp>

#define IR_REC_PIN 2
#define UP 0x18
#define DOWN 0x52
#define LEFT 0x8
#define RIGHT 0x5A
#define STAR 0x16

void led_up(int j);
void led_down(int j);
void led_left(int i);
void led_right(int i);
void led_write();
void turn_on_off();
int get_posi();
int get_posj();

                                // pin 00=3, 01=4, 10=5, 11=6
int states[2][2] = { 1, 0       // i means vertical pos
                   , 0, 0 };    // j means horizontal

void setup()
{
    for (int i = 3; i < 7; i++) pinMode(i, OUTPUT);
    IrReceiver.begin(IR_REC_PIN, ENABLE_LED_FEEDBACK);
}

void loop()
{
    if (IrReceiver.decode()) {
        uint16_t command = IrReceiver.decodedIRData.command;
        int i = get_posi();
        int j = get_posj();
        if (i == -1 && j == -1) {
            for(;;){
                digitalWrite(3, 1);
                delay(90);
                digitalWrite(3, 0);
                digitalWrite(6, 1);
                delay(90);
                digitalWrite(6, 0);
            }
        }
        else if (command == STAR) turn_on_off();
        else if (command == UP) led_up(j);
        else if (command == DOWN) led_down(j);
        else if (command == LEFT) led_left(i);
        else if (command == RIGHT) led_right(i);
        led_write();
        delay(1);
        IrReceiver.resume();
    }
}

void led_up(int j) {
    if (states[0][j]) return; // regardless of horizontal position j can't move up
    else {
        states[1][j] = 0;
        states[0][j] = 1;
    }
}

void led_down(int j) {
    if (states[1][j]) return;
    else {
        states[0][j] = 0;
        states[1][j] = 1;
    }
}

void led_right(int i) {
    if (states[i][1]) return;
    else {
        states[i][0] = 0;
        states[i][1] = 1;
    }
}

void led_left(int i) {
    if (states[i][0]) return;
    else {
        states[i][1] = 0;
        states[i][0] = 1;
    }
}

void led_write() { // pin 00=3, 01=4, 10=5, 11=6
    digitalWrite(3, states[0][0]);
    digitalWrite(4, states[0][1]);
    digitalWrite(5, states[1][0]);
    digitalWrite(6, states[1][1]);
}

void turn_on_off() {
    int i = get_posi();
    int j = get_posj();
    int command = 1;
    states[i][j] = !states[i][j];
    led_write();
    do
    {
        IrReceiver.resume();
        if (IrReceiver.decode()) {
            command = IrReceiver.decodedIRData.command;
            delay(250);
        }
    } while (command != STAR);
    
    states[i][j] = !states[i][j];
}

int get_posi() {
    for (size_t i = 0; i < 2; i++)
    {
        for (size_t j = 0; j < 2; j++)
        {
            if (states[i][j]) return i;
        }   
    }
    return -1;
}

int get_posj() {
    for (size_t i = 0; i < 2; i++)
    {
        for (size_t j = 0; j < 2; j++)
        {
            if (states[i][j]) return j;
        }   
    }
    return -1;
}
