from time import sleep
from FT_Device import *
from FT_declarations import *
from bitarray import bitarray
#
class MSP430:
   
   WAIT_TIME = 0.001

   def __init__(self,ft_dev):
      #self.WAIT_TIME = 2
      self.ftdev = ft_dev
      
   def openDevice():
      try:
         ft_dev = FT_Device.openDevice()
      except D2XXException as e:
         raise e
      return MSP430(ft_dev)
      
   def __bootloader_init__(self):
      self.ftdev.setDTR()
      self.ftdev.setRTS()
      sleep(MSP430.WAIT_TIME)
      self.ftdev.clearRTS()
      sleep(MSP430.WAIT_TIME)
      self.ftdev.setRTS()
      sleep(MSP430.WAIT_TIME)
      self.ftdev.clearRTS()
      sleep(MSP430.WAIT_TIME)
      self.ftdev.clearDTR()
      sleep(MSP430.WAIT_TIME)
      self.ftdev.setRTS()
      sleep(MSP430.WAIT_TIME)
      self.ftdev.clearDTR()
      self.ftdev.clearRTS()
      self.ftdev.setDataCharacteristics(FT_WordLength.FT_BITS_8, FT_StopBits.FT_STOP_BITS_1, FT_Parity.FT_PARITY_EVEN )
      self.ftdev.setBaudrate(9600)
   
   def reset(self):
      self.ftdev.setDTR()
      self.ftdev.setRTS()
      sleep(MSP430.WAIT_TIME)
      self.ftdev.clearDTR()
      sleep(MSP430.WAIT_TIME)
      self.ftdev.clearDTR()
      self.ftdev.clearRTS()
      
      
   
   def __sync_seq__(self):
      self.ftdev.write([128])
      print(self.ftdev.read(1))
      
   def program(self, file):
      pass
      
   def __str__ (self):
      return "MSP430: \nFT_Handle: <{}>".format(self.ftdev.getHandle())
 
  
def testRoutine():  

   # zahl = bitarray("10000000", endian='little')
   # print(zahl.tobytes())
   # zahl = bitarray("10000000", endian='big')
   # print(zahl.tobytes())
   try:     
      msp = MSP430.openDevice()
   except D2XXException as err:
      print(err.message)
      return
   print(msp)
   
   msp.ftdev.check_endian(8)
   
   msp.reset()
   input("bootloader starten:")
   msp.__bootloader_init__()
   msp.__sync_seq__()
   print("fertig =)")
testRoutine()