#include "button_press_queue.h"
#include <Arduino.h>

ButtonPressQueue::ButtonPressQueue(Joystick_* joystick) : 
                            _joystick(joystick), 
                            _queue(), 
                            _lastPressedButton(nullptr),
                            _storage_array()
{
    _queue.setStorage(_storage_array);
}

ButtonPressQueue::~ButtonPressQueue()
{
    _joystick = nullptr;
    _queue.clear();
}

void ButtonPressQueue::queueButtonPress( uint8_t button )
{
    printButtonMsg("Queuing a press for button: ", button);
    _queue.push_back(button);
}

void ButtonPressQueue::processQueue()
{
    // If we dont have button to release
    if (_lastPressedButton == nullptr)
    {
        // If queue has items
        if ( _queue.size() > 0 )
        {
            _lastPressedButton = &_queue.front();
            printButtonMsg("Pressing the button from the queue: ", *_lastPressedButton);
            _joystick->pressButton(*_lastPressedButton);
            delay(100);
        }
    }
    else
    {
        printButtonMsg("Releasing the button from the queue: ", *_lastPressedButton);
        _joystick->releaseButton(*_lastPressedButton);
        //Check that we have element to remove
        if (_queue.size() > 0)
            _queue.remove(0);
        Serial.print("Size of the queue: ");
        Serial.println(_queue.size());
        _lastPressedButton = nullptr;
    }
}

void ButtonPressQueue::printButtonMsg(const char* msg, uint8_t button) const
{
    Serial.print(msg);
    Serial.println(button);
}