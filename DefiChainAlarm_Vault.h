#ifndef DEFICHAINALARM_VAULT
#define DEFICHAINALARM_VAULT

#include "Arduino.h"

class DefiChainAlarm_Vault
{
  public:
    DefiChainAlarm_Vault();
  
    void  SetLimit(int Limit);
    int   GetLimit(void);
    char* GetVaultInfoLink(void);
    char* TestVaultStatus(int nextVaultRatio);
    
  private:
    int _VaultLimit;
    int _TestRetryCounter;
};

#endif
