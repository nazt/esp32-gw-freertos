#include <rom/rtc.h>
#include <Arduino.h>
void print_reset_reason(RESET_REASON reason, HardwareSerial* s)
{
  switch ( reason)
  {
    case 1 : s->println ("POWERON_RESET");break;          /**<1,  Vbat power on reset*/
    case 3 : s->println ("SW_RESET");break;               /**<3,  Software reset digital core*/
    case 4 : s->println ("OWDT_RESET");break;             /**<4,  Legacy watch dog reset digital core*/
    case 5 : s->println ("DEEPSLEEP_RESET");break;        /**<5,  Deep Sleep reset digital core*/
    case 6 : s->println ("SDIO_RESET");break;             /**<6,  Reset by SLC module, reset digital core*/
    case 7 : s->println ("TG0WDT_SYS_RESET");break;       /**<7,  Timer Group0 Watch dog reset digital core*/
    case 8 : s->println ("TG1WDT_SYS_RESET");break;       /**<8,  Timer Group1 Watch dog reset digital core*/
    case 9 : s->println ("RTCWDT_SYS_RESET");break;       /**<9,  RTC Watch dog Reset digital core*/
    case 10 : s->println ("INTRUSION_RESET");break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : s->println ("TGWDT_CPU_RESET");break;       /**<11, Time Group reset CPU*/
    case 12 : s->println ("SW_CPU_RESET");break;          /**<12, Software reset CPU*/
    case 13 : s->println ("RTCWDT_CPU_RESET");break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : s->println ("EXT_CPU_RESET");break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : s->println ("RTCWDT_BROWN_OUT_RESET");break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : s->println ("RTCWDT_RTC_RESET");break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : s->println ("NO_MEAN");
  }
}

void verbose_print_reset_reason(RESET_REASON reason, HardwareSerial* s) {
  switch ( reason)
  {
    case 1  : s->println ("Vbat power on reset");break;
    case 3  : s->println ("Software reset digital core");break;
    case 4  : s->println ("Legacy watch dog reset digital core");break;
    case 5  : s->println ("Deep Sleep reset digital core");break;
    case 6  : s->println ("Reset by SLC module, reset digital core");break;
    case 7  : s->println ("Timer Group0 Watch dog reset digital core");break;
    case 8  : s->println ("Timer Group1 Watch dog reset digital core");break;
    case 9  : s->println ("RTC Watch dog Reset digital core");break;
    case 10 : s->println ("Instrusion tested to reset CPU");break;
    case 11 : s->println ("Time Group reset CPU");break;
    case 12 : s->println ("Software reset CPU");break;
    case 13 : s->println ("RTC Watch dog Reset CPU");break;
    case 14 : s->println ("for APP CPU, reseted by PRO CPU");break;
    case 15 : s->println ("Reset when the vdd voltage is not stable");break;
    case 16 : s->println ("RTC Watch dog reset digital core and rtc module");break;
    default : s->println ("NO_MEAN");
  }
}
