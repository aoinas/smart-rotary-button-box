#include <Arduino.h>
//#include <Keyboard.h>
#include <Joystick.h>
#include "CustomRotaryEncoder.h"

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

int getEncoderCount()
{
    return sizeof(encoders)/sizeof(encoders[0]);
}

MODE mode = NOT_DEFINED;
unsigned long previousStatusTime = 0;
int statusInterval = 30 * 1000; // Every 10 seconds


// the setup function runs once when you press reset or power the board
void setup()
{
    // initialize digital pin LED_BUILTIN as an output.
    //pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_MODE_SWITCH, INPUT);
    pinMode(PIN_BACK_BUTTON, INPUT);

    pinMode(4, INPUT); // Encoder clicks // TODO: move to CustomRotaryEncoder
    pinMode(7, INPUT); // Encoder clicks // TODO: move to CustomRotaryEncoder
    pinMode(21, INPUT); // Encoder clicks // TODO: move to CustomRotaryEncoder
    pinMode(14, INPUT); // Encoder clicks // TODO: move to CustomRotaryEncoder
    pinMode(10, INPUT); // Encoder clicks // TODO: move to CustomRotaryEncoder
    

    Serial.begin(57600);
    Joystick.begin(true);
    

    
}
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
            Joystick.pressButton(joyBtn);
        else
            Joystick.releaseButton(joyBtn);
    }

/*
    if (millis() - previousStatusTime > statusInterval)
    {
        printStatus();
        previousStatusTime = millis();
    }
*/
    /*
   // Blinking led
  int delayInMs = mode ? 200 : 1000;
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(delayInMs);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(delayInMs);                       // wait for a second
  */

    //delay(10); // Just some delay so we dont run too fast :)
}

void setEncoderDirections()
{
    for (int i = 0; i < getEncoderCount(); i++)
    {
        encoders[i].tick();
        encoders[i].myTick();

        RotaryEncoder::Direction direction = encoders[i].getDirection();
        if (direction != RotaryEncoder::Direction::NOROTATION)
        {
            ENCODER_ACTION action = direction == RotaryEncoder::Direction::CLOCKWISE ? ENCODER_ACTION::INCREASE : ENCODER_ACTION::DECREASE;
            uint8_t button = encoders[i].getButtonIndex(mode == SIM, action);
            Joystick.pressButton(button);
            delay(100); // Debug
            Joystick.releaseButton(button);

            Serial.print("Encoder action: ");
            Serial.println(action);
        }

        if (  encoders[i].getStateChanged() )
        {
            uint8_t button = encoders[i].getButtonIndex(mode == SIM, ENCODER_ACTION::CLICK);
            if (  encoders[i].getIsPressed() )
            {
                //executeClickFunction(i);
                Joystick.pressButton(button);
            }
            else
                Joystick.releaseButton(button);
        }
            
    }
}

void setMode(int newMode)
{
    if (mode != newMode)
    {
        mode = newMode == SIM ? SIM : SYSTEM;

        // Release buttons
        Joystick.releaseButton(joystickButtonIndex_Sim_Back);
        Joystick.releaseButton(joystickButtonIndex_System_Back);

        printStatus();
    }
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

void executeFunction(int encoderIndex, RotaryEncoder::Direction direction)
{
    /*
    unsigned int keyIndex = direction == RotaryEncoder::Direction::COUNTERCLOCKWISE ? 0 : 1;
    keyIndex += mode == SIM ? 0 : 3;
    char keyboardCmd = encoders[encoderIndex].getKey(keyIndex);

    Serial.print("Encoder #");
    Serial.print(encoderIndex+1);
    Serial.print(": Key command: ");
    Serial.println( keyboardCmd );
*/
    //Keyboard.press(keyboardCmd);
    //Keyboard.releaseAll();
}

void executeClickFunction(int encoderIndex)
{
    /*
    unsigned int keyIndex = 2;
    keyIndex += mode == SIM ? 0 : 3;
    char keyboardCmd = encoders[encoderIndex].getKey(keyIndex);

    Serial.print("Encoder #");
    Serial.print(encoderIndex+1);
    Serial.print(" clicked: ");
    Serial.print(" Key command: ");
    Serial.println( keyboardCmd );
*/

}
