#include <Arduino.h>

#if !defined(_DIGITALINPUT)
#define _DIGITALINPUT


class digitalInput
{
private:
    uint8_t _pin = -1;
    bool _state;
    bool _previousState;
    uint8_t _mode;
    int _debonceTime = 10;
    unsigned long lastDebounceTime = 0;
    unsigned long pressedSince = 0;
public:
    digitalInput();
    digitalInput(uint8_t pin,uint8_t mode);
    ~digitalInput();
    bool begin();
    void loop();
    bool isPressed();
    bool isReleased();
    bool frontMontant();
    bool frontDesceandant();

    /**
     * @brief retourne le temps en ms d'appui
     * 
     * @return unsigned long 
     */
    unsigned long pressedTime();

    bool getState();
};




#endif // _DIGITALINPUT
