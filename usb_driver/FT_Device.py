from ctypes import *
from FT_declarations import *
d2xx = WinDLL('ftd2xx')


class D2XXException (Exception):
   def __init__(self, message):
      self.message = message


class FT_Device(object):
   def __init__(self,id):
      self.handle = id
      self.endian = 'big'
      
   def openDevice(dev_id=0):
      handle = c_int()
      state = d2xx.FT_Open(dev_id,byref(handle))
      if (FT_State(state) != FT_State.FT_OK):
         raise D2XXException("Could not Open Device! - {}".format(FT_State(state)))
      device = FT_Device(handle)
      return device
     
   def setDataCharacteristics(self, wordLength, stopBits, parity):
      state = d2xx.FT_SetDataCharacteristics(self.handle,c_ubyte(wordLength.value),c_ubyte(stopBits.value),c_ubyte(parity.value))
      if (FT_State(state) != FT_State.FT_OK):
         raise D2XXException("Configuration ERROR - {}".format(FT_State(state)))
   
   # big or little
   def setEndian(self,endian):
      self.endian = endian
      
     
   def close(self):
      d2xx.FT_Close(self.handle)
   
   def setDTR(self):
      self.__setControlFlag(d2xx.FT_SetDtr,"set DTR")
   
   def clearDTR(self):
      self.__setControlFlag(d2xx.FT_ClrDtr,"clear DTR")
   
   def setRTS(self):
      self.__setControlFlag(d2xx.FT_SetRts,"set RTS")
     
   def clearRTS(self):
      self.__setControlFlag(d2xx.FT_ClrRts,"clear RTS")
   
   def __setControlFlag(self, fkt, errormsg):
      state = fkt(self.handle)
      if (FT_State(state) != FT_State.FT_OK):
         raise D2XXException("Could not {}! - {}".format(errormsg, FT_State(state)))
   
   def read(self,n_bytes):
      buffer = create_string_buffer(n_bytes)
      count = c_int()
      state = d2xx.FT_Read(self.handle,buffer,n_bytes,byref(count))
      if (FT_State(state) != FT_State.FT_OK):
         raise D2XXException("READ ERROR - {} | {} bytes read.".format(FT_State(state),count))
      return buffer.raw[:count.value]
   
   
   #returns incomming char as int in correct order for self.endian
   def check_endian(self,char):
      if(self.endian == "big"):
         return self.swap_bits(char)
      return int("".join(self.getBinArray(char)),2)   # string or int to int!
     
   #returns incomming char as integer.
   def swap_bits(self,char):

      old = self.getBinArray(char)        #convert incoming char to binary array
      cLength = len(old)               
      new = [None] * cLength;             #make a new list with reversed data [1,2,3] -> [3,2,1]
      for i in range(cLength):
         new[i] = old[(cLength-1) - i]    #swap them
      lsgString = "".join(new)            #make array to string
      return int(lsgString,2)             #binärzahl
   
   #ret
   def getBinArray(self,char):
      if(type(char) is int):
         return list(bin(char))[2:]
      if(type(char) is str):
         return list((bin(ord(char)))[2:])
      print("TYPE ERROR")
      return['0','0']

   def write(self, data):
      n_bytes = len(data)
      print(data)
      buffer = create_string_buffer(n_bytes)
      for i in range(n_bytes):
         buffer[i] = c_char(data[i].to_bytes(1, byteorder=self.endian))
         print(buffer[i])
      count = c_int()
      state = d2xx.FT_Write(self.handle,buffer,n_bytes,byref(count))
      if (FT_State(state) != FT_State.FT_OK):
         raise D2XXException("READ ERROR - {} | {} bytes written.".format(FT_State(state),count))
      return count.value
   
   def writeString(self,data):
      self.write(data.encode('utf-8'))
   
   def __str__ (self):
      return "HANDLE: <{}>".format(self.handle)
      
   def getHandle(self):
      return self.handle
      
   def setBaudrate(self, baud):
      state = d2xx.FT_SetBaudRate(self.handle, baud)
      if (FT_State(state) != FT_State.FT_OK):
         raise D2XXException("Set Baudrate ERROR - {} | for Baudrate = {}".format(FT_State(state),baud))

# TEST     
def testRoutine():
   try:
      device = FT_Device.openDevice()
   except D2XXException as err:
      print(err.message)
      return
   print(device)
   input("set DTR")
   device.setDTR()
   input("clr DTR")
   device.clearDTR()
   input("set RTS")
   device.setRTS()
   input("clr RTS")
   device.clearRTS()
   device.write("ABCDEFG".encode('utf-8'))
   device.writeString("ABCDEFGHIJKL")
   device.close()

def test_Endian():
   big = FT_Device(1)
   little = FT_Device(1)
   ##!WICHTIG!##
   #1.
   #check_endian darf bisher nur mit int oder einzelnem char angesprochen werden NICHT MIT STRING!
   #lsg = schleife welche String charweise an fkt übergibt und später wieder zusammen klebt
   #2.
   #wenn Zahl als char übergeben wird, zahl zu int umwandeln oder als char behalten ?
   #also '5' = 0b0101 oder 0b110101 ?
   little.endian="little"
   print("128 (dezimal)\tbig: {}  \tlittle: {}".format(big.check_endian(128),little.check_endian(128)))
   print("'5' ({})\tbig: {}  \tlittle: {}".format(bin(ord('5'))[2:],big.check_endian('5'),little.check_endian('5')))
   for i in range(97,123):
      j = chr(i)
      print("'{}' ({})\tbig: {}  \tlittle: {}".format(j, bin(ord(j))[2:],big.check_endian(j),little.check_endian(j)))



test_Endian()
#input("fertig =)")
