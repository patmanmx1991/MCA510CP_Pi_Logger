#include "command.hpp"
extern "C"
{
  USBGammaSpec* USBGammaSpec_ctype(unsigned short id) { return new USBGammaSpec(id); };


  void StartRun_ctype(USBGammaSpec* daq) { daq->StartRun(); };
  void StopRun_ctype(USBGammaSpec* daq) { daq->StopRun(); };
  void GetReadings_ctype(USBGammaSpec* daq) { daq->GetReadings(); };
  long GetReadingExposure_ctype(USBGammaSpec* daq) { daq->GetReadingExposure(); };
  int GetCounts_ctype(USBGammaSpec* daq, int bin){ return daq->GetCounts(bin); };

  void SetConfig_ctype(USBGammaSpec* daq,
                       unsigned short hv=550,
                       unsigned short gain=1000,
                       unsigned short lld=25,
                       wchar_t in = 1,
                       wchar_t pmtgain = 12,
                       wchar_t out1 = 1,
                       wchar_t out2 = 1){ daq->SetConfig( hv, gain, lld, in, pmtgain, out1, out2 ); };
  
}
  
