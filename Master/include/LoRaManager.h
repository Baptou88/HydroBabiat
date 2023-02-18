#include <Arduino.h>
#include <LList.h>
#include <main.h>
#include <LoRa.h>


#if !defined(__LoRaManager__)
#define __LoRaManager__

    
    

    class LoRaManager
    {
    private:
        LList<nodeStatus_t*> listNodes;
        int nodeChecked = 0;
    public:
        LoRaManager(/* args */);
        ~LoRaManager();
        void loop();
    };
    
    
    

#endif