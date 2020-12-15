
#ifndef CustomRotaryEncoder_h
#define CustomRotaryEncoder_h

#include <RotaryEncoder.h>

enum ENCODER_ACTION
{
    DECREASE = 0,
    INCREASE = 1,
    CLICK = 2
};

class CustomRotaryEncoder : public RotaryEncoder
{
    public:
        // ----- Constructor -----
        CustomRotaryEncoder(int lessPin, int morePin, int clickPin, 
                            uint8_t firstButtonIndex ) : RotaryEncoder(lessPin, morePin)
        {
            _clickPin = clickPin;
            _isPressed = false;
            _firstButtonIndex = firstButtonIndex;
        }

        uint8_t getButtonIndex(bool primary, ENCODER_ACTION action)
        {
            uint8_t base = primary ? _firstButtonIndex : _firstButtonIndex+3;
            return base + (uint8_t)action;
        }

        int getClickPin()
        {
            return _clickPin;
        }

        void myTick()
        {
            bool newState = digitalRead( getClickPin() ) == LOW;
            _stateChanged = _isPressed != newState;
            if ( _stateChanged )
                _isPressed = newState;
        }

        bool getStateChanged()
        {
            return _stateChanged;
        }

        bool getIsPressed()
        {
            return _isPressed;
        }

    private:
        int _clickPin;
        uint8_t _firstButtonIndex;
        bool _isPressed;
        bool _stateChanged;

};

#endif