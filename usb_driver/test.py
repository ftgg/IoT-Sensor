from MSP430 import *

try:     
   msp = MSP430.openDevice()
except D2XXException as err:
   print(err.message)

while 1:
   print(msp.read(4))