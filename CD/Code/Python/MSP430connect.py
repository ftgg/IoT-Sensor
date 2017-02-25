import os.path
from MSP430 import *

def cli(msp, showMsg=""):
   cmd = input(showMsg)
   
   retval = __handle_input(cmd)
   
   if(retval == -1):
      return -1
  
def __handle_input(cmdString):
   if(len(cmdString) < 1):
      return 0
      
   if(cmdString == 'hd'):
      HELP_STR = "Programm erzeugen:\n\n"
      HELP_STR += "\tes muss der Pfad zu einer .txt datei führen,\n\twelche dem TI_TXT hex format entspricht\n\n"
      HELP_STR += "Um diese zu erzeugen in den Projekteinstellungen folgendes tun:\n"
      HELP_STR += "\t1. unter 'Build' Configuration auf Release stellen\n"
      HELP_STR += "\t2. unter 'MSP430 Hex Utility' haken bei Enable MSP430 \n\t   Hex Utility aktivieren\n"
      HELP_STR += "\t3. unter 'Output Format Options' das Output format auf \n\t   Output TI_TXT hex format setzen\n"
      HELP_STR += "\t4. Build Release ausführen, 'projektname'.txt liegt jetzt im workspace\n"
      print(HELP_STR)
      return 1
      
   if cmdString[0] == "h":
      HELP_STR = "Befehle:\n\n"
      HELP_STR += "e\tBeendet das Programm\n"
      HELP_STR += "d path\tLäd das Programm unter 'path' auf den MSP430\n"
      HELP_STR += "\thilfe zum erstellen der Datei erhalten sie mit Eingabe 'hd'\n"
      print(HELP_STR)
      return 1
      
   if cmdString[0] == "e":
      return -1
      
   if cmdString[0] == "d":
      code, msp = __download(cmdString)
      MSP = msp
      return code

def __download(str):
   if(len(str) < 7):
      print("Pfadangabe nötig (d C:/Users/.../out.txt)")
      return 0, None
      
   if str[-4:] != ".txt":
      print("ein .txt file wird benötigt tippe hd für hilfe")
      return 0, None
   
   str = str.replace("\\","/")
   
   f = None
   if not os.path.isfile(str[2:]):
      print("{} konnte nicht gefunden werden".format(str[2:]))
      return -2, None

   try:
      msp.program(str[2:])
   except:
      return -3, None
   print("Download erfolgreich!")
   return 1,msp

def __write(data):
   pass
   
#def write(self, data):
#      return self.ftdev.write(data)
#   
#   def read(self, n_bytes):
#      return self.ftdev.read(n_bytes)

def main():
   WELCOME_STR = "MSP430 connector stellt eine Verbindung zu ihrem MSP430 her,\nmit h gelangen sie zur Hilfe\n\n"
   
   msp = None
   try:     
      msp = MSP430.openDevice()
   except Exception as err:
      print(err)
      return -1
   
   retval = cli(msp,WELCOME_STR)
   while retval != -1:
      retval = cli(msp)
main()