/* function to read and monitor inverter dc-link */

;use variable VDC1 and VDC2
;use variable DC_balanceP and DC_balanceN
;use variable DCLINK_MAX


#include "p30f6010A.inc"

.global _asmDClink

_asmDClink:
disi #0x3FFF

MOV #0x0303, W0    ;read VDC1 adc channel 3
CALL _asmADC
MOV W0, W7         ;VDC1 in W7

MOV #0x0202, W0    ;read VDC2 adc channel 2
CALL _asmADC
MOV W0, W8         ;VDC2 in W8

;detect peak and set fault
MOV _DCLINK_MAX, W5 ;read VDC max trip limit
CP W7,W5            ;compare VDC1 or initiate trip
BRA GT,Trip111
CP W8,W5            ;compare VDC2 or initiate trip
BRA GT,Trip111

BRA Last3

Trip111:
BSET _IFS2+1,#4 ;set if fault 

Last3:
disi #0x0000

return 
.end

