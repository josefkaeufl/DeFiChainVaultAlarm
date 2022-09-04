#ifndef DEFICHAINALARM_HTTP
#define DEFICHAINALARM_HTTP

#include "Arduino.h"

class DefiChainAlarm_Http
{
  public:
    void GetVaultStatus(int* VaultRatio, int* nextVaultRatio);
    void GetDfiPrice(float* DfiPrice);
    void GetDusdFee(float* DusdFee);
    
  private:

};



#endif
