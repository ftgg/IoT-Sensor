from time import sleep
from FT_Device import *
from FT_declarations import *
from bitarray import bitarray
from ProgrammContainer import *
import math as math
#

class BslVersion():
   def __init__(self,vendor,cmd_intptr,api,p_int):
      self.vendor = vendor
      self.cmd_intptr = cmd_intptr
      self.api = api
      self.p_int = p_int
   
   def __str__(self):
      return ("BSL VERSION:\nVendor:\t\t\t{}\nCommand Interpreter:" + 
      "\t{}\nAPI:\t\t\t{}\nPeripheral Interface:   {}\n").format(self.vendor,self.cmd_intptr,self.api,self.p_int)     


class MSP430Exception (Exception):
   def __init__(self, message):
      self.message = message
      
class MSP430:
   BUFFER_SIZE = 260
   WAIT_TIME = 0.01

   def __init__(self,ft_dev):
      self.ftdev = ft_dev
      self.bootloader_init = False
      self.unlocked = False
      self.bslBase = 'ROM'
      self.__configUART()
   
   def __str__ (self):
      data = "-----\nMSP430: \nFT_Handle: <{}>\nDevice unlocked: {}".format(self.ftdev.getHandle(),self.unlocked)
      if self.bootloader_init:
         data = data + "\n{}".format(self.getBslVersion())
      return data + "-----\n"

   
   def openDevice():
      try:
         ft_dev = FT_Device.openDevice()
      except D2XXException as e:
         raise MSP430Exception(e.message)
      return MSP430(ft_dev)
   
   
   def reset(self):
      self.ftdev.setDTR()
      self.ftdev.setRTS()
      sleep(MSP430.WAIT_TIME)
      self.ftdev.clearDTR()
      sleep(MSP430.WAIT_TIME)
      self.ftdev.clearDTR()
      self.ftdev.clearRTS()
           
   # TX BSL Version
   def getBslVersion(self):
      if self.bootloader_init != True:
         self.__bootloader_init()
         
      if self.unlocked != True:
         self.__unlock_device()

      cmd = [0x80,1,0,0x19]           # HDR LENGTH LENGTH CMD(0x19) CKL CKH
      cmd = self.__addCheckSumToCmd(cmd)
      
      self.write(cmd)
      data = self.read(11)
      if(data[0] != 0x00):
         raise MSP430Exception("ERROR in getBslVersion")
        
      return BslVersion(data[5],data[6],data[7],data[8])
      
   def program(self, filepath):
      if self.bootloader_init != True:
         self.__bootloader_init()
      progContainer = ProgramContainer.loadProgram(filepath)
      progData = progContainer.getData()
      self.__massErase()
      for dataBlock in progData:
         self.__downloadDataBlock(*dataBlock)
         writtenData = self.uploadDataBlock(dataBlock[0],dataBlock[2])
         if(not self.__verifyPrograming(*dataBlock)):
            raise MSP430Exception("Programming ERROR at Addr: {}".format(dataBlock[0]))
      self.reset()              
    
   def write(self, data):
      return self.ftdev.write(data)
   
   def read(self, n_bytes):
      return self.ftdev.read(n_bytes)
      
   # TX DATA BLOCK
   def uploadDataBlock(self,addr,length):
      if self.bootloader_init != True:
         self.__bootloader_init()
      if self.unlocked != True:
         self.__unlock_device()

         
      # Requested Bytes
      requ_bytes = self.__getLengthBytes(length)
      # Data Length
      len_bytes = self.__getLengthBytes(0x06)
      # Adress
      addr_bytes = self.__getAdressBytes(addr)
      
      # Checksum
      cmd = [0x80,*len_bytes,0x18,*addr_bytes,*requ_bytes]
      cmd = self.__addCheckSumToCmd(cmd)                   # Header Length Length CMD AL AM AH D1 ... DN(DataLength 2 Bytes) CKL CKH
      
      # Write
      self.write(cmd)

      #rvc multiple Packages
      N_HDR_ELEMENTS = 7
      n_blocks = math.ceil(length / (MSP430.BUFFER_SIZE - 1))
      expectedBytes = length + n_blocks * N_HDR_ELEMENTS
      dataBlocks = self.read(expectedBytes)
      return self.__combineDataBlocks(dataBlocks,n_blocks)

# ---------------- Private Methods  --------------------------------- #     
   def __combineDataBlocks(self,dataBlocks,n_blocks):
      N_HDR_ELEMENTS = 7
      data = []
      for i in range(n_blocks):
         segmentLen = (len(dataBlocks) - N_HDR_ELEMENTS) / (MSP430.BUFFER_SIZE - 1)
         if segmentLen >= 1:  
            data = [*data, *dataBlocks[5:5 + MSP430.BUFFER_SIZE - 1]]               # This case is not properly tested!
            dataBlocks = dataBlocks[N_HDR_ELEMENTS + MSP430.BUFFER_SIZE - 1:]
         else:
            data = [*data, *dataBlocks[5:-2]]
      return data
 
   def __bootloader_init(self):
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
      self.bootloader_init = True

   def __configUART (self):
      self.ftdev.setDataCharacteristics(FT_WordLength.FT_BITS_8, FT_StopBits.FT_STOP_BITS_1, FT_Parity.FT_PARITY_EVEN )
      self.ftdev.setBaudrate(9600)
      self.ftdev.setEndian("big")
 
   # RX Data Block
   def __downloadDataBlock(self,addr, code, length):
      if self.unlocked != True:
         self.__unlock_device()

      # Data Length
      len_bytes = self.__getLengthBytes(length + 4)
      # Adress
      addr_bytes = self.__getAdressBytes(addr)
      
      # Checksum
      cmd = [0x80,*len_bytes,0x10,*addr_bytes,*code]
      cmd = self.__addCheckSumToCmd(cmd)                   # Header Length Length CMD AL AM AH D1 ... DN CKL CKH
      
      # Write
      self.write(cmd)
      
      # get Response
      resp = self.read(8)
      if(resp[4] != 0x3B or resp[5] != 0x00):
         raise MSP430Exception("Doenloading Error! <{}>".format(resp))
 
 
 
   def __massErase(self):
      chk = self.__calculate_checksum([0x15])
      self.write([0x80,0x01,0x00,0x15,*chk])   # HDR LL LH CMD(0x15) CKL CKH
      self.read(8)

   def __verifyPrograming(self, addr, data, length):
      chckdev = self.__crcCheck(addr, length)
      chcksel = self.__calculate_checksum(data)
      if(chckdev == chcksel):
         return True
      return False
      
      
   # CRC Check
   def __crcCheck(self, addr, length):
      addr_bytes = self.__getAdressBytes(addr)
      requ_bytes = self.__getLengthBytes(length)
      cmd = [0x80,0x06, 0x00, 0x16, *addr_bytes, *requ_bytes] # HDR LENGTH LENGTH CMD(0x16) D1=Length D2=Length CKL CKH
      cmd = self.__addCheckSumToCmd(cmd)
      self.write(cmd)
      data = self.read(9)
      return (data[5],data[6])
           
           
   # RX Password
   def __unlock_device(self,pwd=[0xFF] * 32): 
      if(len(pwd) < 32):
         return False
      cmd = [0x80,0x21,0x0,0x11,*pwd]              # HDR LENGTH LENGTH CMD(0x11) D1 ... D32 CKL CKH
      cmd = self.__addCheckSumToCmd(cmd)
      self.write(cmd)
      resp = self.read(8)
      self.unlocked = True
   
   
   def __getLengthBytes(self,length):
      ll = length & 0xFF
      lh = (length >> 8) & 0xFF
      return (ll,lh)
   
   
   def __getAdressBytes(self, addr):
      al = addr & 0xFF
      am = (addr >> 8) & 0xFF
      ah = (addr >> 16) & 0xFF
      return (al,am,ah)
   
   
   def __addCheckSumToCmd(self,cmd):
      chk = self.__calculate_checksum(cmd[3:])
      return [*cmd,*chk]
      
   def __calculate_checksum(self, data):
      if self.bslBase == 'ROM':
         return self.__calcChecksumROM(data)
      else:
         return self.__calcChecksumFLASH(data)
      
   def __calcChecksumFLASH(self, data):
      # FLASH BASED
      ckh = 0
      ckl = 0
      for i in range(len(data)):
         if i % 2 == 0:
            ckl ^= data[i] 
         else:
            ckh ^= data[i]
         
         ckl ^= 0xFF
         ckh ^= 0xFF
      return (ckl,ckh)
      
   def __calcChecksumROM(self, data):
      # ROM BASED
      crc = 0xFFFF
      for i in range(len(data)):
         x = ((crc>>8) ^ data[i]) & 0xFF
         x ^= x>>4
         crc = (crc <<8) ^ (x <<12) ^ (x << 5) ^ x
         
      return ((crc & 0xFF),(crc >> 8) & 0xFF)    
 

# ---------------- TEST Routine  --------------------------------- #
def testRoutine():  
   try:     
      msp = MSP430.openDevice()
   except D2XXException as err:
      print(err.message)
      return
   msp.program("C:/Users/edc42/IoT-Sensor/usb_driver/real_blink.txt")
   input("fertig =)")
 
# testRoutine()