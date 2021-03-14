#include <Joystick.h>
#include <Vector.h>

class ButtonPressQueue
{
    public:
    ButtonPressQueue(Joystick_* joystick);
    ~ButtonPressQueue();
    void queueButtonPress(uint8_t button);
    void processQueue();

    private:
    Joystick_* _joystick;
    Vector<uint8_t> _queue;
    uint8_t* _lastPressedButton;
    uint8_t _storage_array[100];

    void printButtonMsg(const char* msg, uint8_t button) const;
};