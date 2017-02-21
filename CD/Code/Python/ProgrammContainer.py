from math import ceil

class ProgramContainer:  
   def __init__(self, data):
      self.__data = data # [(Adress, Data, len)]
    
   def loadProgram(path):
      with open(path, 'r') as f:
         dataString = f.read()
      dataBlocks = dataString.split('@')
      data = ProgramContainer.__parseBlocks(dataBlocks[1:])
      return ProgramContainer(data)   

   def __parseBlocks(blocks):
      data = []
      for b in blocks:
         data = [*data,*ProgramContainer.__parseBlock(b)]
      return data
   
   def __parseBlock(block):
      lines = block.split("\n")
      charArray = []
      for e in lines[:-1]:
         charArray.extend(e.split(" "))
      
      byteArray = []
      for c in charArray:
         if c == "" or c == "q":
            continue
         byteArray.append(int(c, 16))
         
      addr = byteArray[0]
      code = byteArray[1:]
      
      returnblock = []

      # seperate blocks in pieces lower than 257
      for i in range(ceil(len(code)/256)):
         codeseg = code[:256]
         returnblock.append((addr+i*256, codeseg, len(codeseg)))
         code = code[256:]

      return returnblock
      
   def getData(self):
      return self.__data
   