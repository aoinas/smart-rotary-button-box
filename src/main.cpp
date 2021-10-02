#include <Arduino.h>
//#include <Keyboard.h>
#include <Joystick.h>
#include "CustomRotaryEncoder.h"
#include "button_press_queue.h"

/*
// Leonardo
#define PIN_MODE_SWITCH 13
#define PIN_BACK_BUTTON 12
*/

// Pro Micro
#define PIN_MODE_SWITCH 0
#define PIN_BACK_BUTTON 1

enum MODE
{
    NOT_DEFINED = HIGH,
    SIM = LOW, // Switch ON (+)
    SYSTEM = HIGH // Switch OFF (-)
};

int backButtonState = 0;
uint8_t joystickButtonIndex_Sim_Back = 0;
uint8_t joystickButtonIndex_System_Back = 1;

CustomRotaryEncoder encoders[5] = {
    CustomRotaryEncoder(2, 3, 4, 2), // Every encoder produces 6 joystick buttons
    CustomRotaryEncoder(5, 6, 7, 8),
    CustomRotaryEncoder(19, 20, 21, 14),
    CustomRotaryEncoder(18, 15, 14, 20),
    CustomRotaryEncoder(8, 9, 10, 26)
};

Joystick_ Joystick( JOYSTICK_DEFAULT_REPORT_ID, 
                    JOYSTICK_TYPE_GAMEPAD, 
                    32,
                    0, // hat switch count
                    false,		//l includeXAxis = true,
                    false,
                    false,
                    false,
                    false,
                    false,
                    false,
                    false,
                    false,
                    false,
                    false
                    );

ButtonPressQueue buttonPressQueue(&Joystick);

int getEncoderCount()
{
    return sizeof(encoders)/sizeof(encoders[0]);
}

MODE mode = NOT_DEFINED;
unsigned long previousStatusTime = 0;
int statusInterval = 30 * 1000; // Every 10 seconds

unsigned long _logMillis = 0;

// the setup function runs once when you press reset or power the board
void setup()
{
    pinMode(PIN_MODE_SWITCH, INPUT);
    pinMode(PIN_BACK_BUTTON, INPUT);

    pinMode(4, INPUT); // Encoder clicks // TODO: move to CustomRotaryEncoder
    pinMode(7, INPUT); // Encoder clicks // TODO: move to CustomRotaryEncoder
    pinMode(21, INPUT); // Encoder clicks // TODO: move to CustomRotaryEncoder
    pinMode(14, INPUT); // Encoder clicks // TODO: move to CustomRotaryEncoder
    pinMode(10, INPUT); // Encoder clicks // TODO: move to CustomRotaryEncoder

    Serial.begin(9600);
    Joystick.begin(true);
    
    _logMillis = millis();
}

void printStatus()
{
    Serial.print(millis() / 1000); // Runtime in seconds
    Serial.print(" STATUS: ");

    Serial.print("   Mode: ");
    Serial.print(mode == SYSTEM ? "SYSTEM" : "CAR");

    Serial.print(" Encoder count: ");
    Serial.print(getEncoderCount());

    Serial.println();

    digitalWrite(LED_BUILTIN, mode == SYSTEM ? HIGH : LOW);
}

void setMode(int newMode)
{
    if (mode != newMode)
    {
        mode = (MODE)newMode;

        // Release buttons
        Joystick.releaseButton(joystickButtonIndex_Sim_Back);
        Joystick.releaseButton(joystickButtonIndex_System_Back);

        printStatus();
    }
}

bool setEncoderDirections()
{
    bool encoderPositionChanged = false;

    for (int i = 0; i < getEncoderCount(); i++)
    {
        encoders[i].tick();
        encoders[i].myTick();

        RotaryEncoder::Direction direction = encoders[i].getDirection();
        if (direction != RotaryEncoder::Direction::NOROTATION)
        {
            ENCODER_ACTION action = direction == RotaryEncoder::Direction::CLOCKWISE ? ENCODER_ACTION::INCREASE : ENCODER_ACTION::DECREASE;
            uint8_t button = encoders[i].getButtonIndex(mode == SIM, action);
            
            buttonPressQueue.queueButtonPress(button);
            encoderPositionChanged |= true;
        }

        if (  encoders[i].getStateChanged() )
        {
            uint8_t button = encoders[i].getButtonIndex(mode == SIM, ENCODER_ACTION::CLICK);
            if (  encoders[i].getIsPressed() )
            {
                Serial.print("Pressing encoder button ");
                Joystick.pressButton(button);
            }
            else
            {
                Serial.print("Releasing encoder button ");
                Joystick.releaseButton(button);
            }
        }
    }

    return encoderPositionChanged;
}


unsigned long _loopCounter = 0;
unsigned long _processQueusMillis = 0;

// the loop function runs over and over again forever
void loop()
{
    setMode(digitalRead(PIN_MODE_SWITCH));
    
    setEncoderDirections();
    
    // Handle back button
    int currBackButtonState = digitalRead(PIN_BACK_BUTTON);

    if ( currBackButtonState != backButtonState)
    {
        backButtonState = currBackButtonState;

        uint8_t joyBtn = mode == MODE::SIM ? joystickButtonIndex_Sim_Back : joystickButtonIndex_System_Back;
        if ( backButtonState == HIGH )
        {
            Serial.print("Pressing back button ");
            Joystick.pressButton(joyBtn);
        }
        else
        {
            Serial.print("Releasing back button ");
            Joystick.releaseButton(joyBtn);
        }
    }

    // Process queue 10 times a second
    if (millis() - _processQueusMillis > 10)
    {
        buttonPressQueue.processQueue();
        _processQueusMillis = millis();
    }

    if (millis() - _logMillis > 15000)
    {
        Serial.print("Loop count: ");
        Serial.println(_loopCounter);
        Serial.print("Avg loop time in ms: ");
        Serial.println( (millis() - _logMillis) / _loopCounter );

        _logMillis = millis();
        _loopCounter = 0;
    }

    _loopCounter++;
    
}









