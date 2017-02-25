from enum import Enum
class FT_State(Enum):
   FT_OK = 0
   FT_INVALID_HANDLE = 1
   FT_DEVICE_NOT_FOUND = 2
   FT_DEVICE_NOT_OPENED = 3
   FT_IO_ERROR = 4
   FT_INSUFFICIENT_RESOURCES = 5
   FT_INVALID_PARAMETER = 6
   FT_INVALID_BAUD_RATE = 7
   FT_DEVICE_NOT_OPENED_FOR_ERASE = 8
   FT_DEVICE_NOT_OPENED_FOR_WRITE = 9
   FT_FAILED_TO_WRITE_DEVICE = 10
   FT_EEPROM_READ_FAILED = 11
   FT_EEPROM_WRITE_FAILED = 12
   FT_EEPROM_ERASE_FAILED = 13
   FT_EEPROM_NOT_PRESENT = 14
   FT_EEPROM_NOT_PROGRAMMED = 15
   FT_INVALID_ARGS = 16
   FT_NOT_SUPPORTED = 17
   FT_OTHER_ERROR = 18
   FT_DEVICE_LIST_NOT_READY = 19

#
class FT_WordLength(Enum):
   FT_BITS_8 = 8
   FT_BITS_7 = 7

   
class FT_StopBits(Enum):
   FT_STOP_BITS_1 = 0 
   FT_STOP_BITS_2 = 2
 
 
class FT_Parity(Enum):
   FT_PARITY_NONE = 0 
   FT_PARITY_ODD = 1
   FT_PARITY_EVEN = 2
   FT_PARITY_MARK = 3
   FT_PARITY_SPACE = 4