/* 
3. Если устройство не мост (0-бит поля Header Type = 0) 
вывести и расшифровать значение полей базовых регистров ввода/вывода. 
 
5. Если устройство не мост (0-бит поля Header Type = 0) 
вывести и расшифровать значение поля Cache Line. 
 
10. Если устройство не мост (0-бит поля Header Type = 1) 
вывести и расшифровать значение полей Memory base, Memory Limit. 
*/ 
#include "stdio.h" 
#include "pci.h" 
#include "stdbool.h" 
#include "sys/io.h" 
#include <math.h> 
#define PRIVILEGE_LEVEL 3 
#define BUS_NUM 256 
#define DEVICE_NUM 32 
#define FUNCTION_NUM 8 
 
#define ENABLE_BIT 0x80000000 
#define VENDOR_ID_OFFSET 0 
#define DEVICE_ID_OFFSET 2 
#define DEVICE_NOT_FOUND_MARKER -1 
#define UNKNOWN_MESSAGE "UNKNOWN" 
#define PCI_VENDORS_TABLE_LENGHT (sizeof(PciVenTable) / sizeof(PCI_VENTABLE)) 
#define PCI_DEVICE_TABLE_LENGHT (sizeof(PciDevTable) / sizeof(PCI_DEVTABLE)) 
 
__uint16_t readPCIDataField(int result, int offset) { 
return (__uint16_t) ((result >> ((offset & 2) * 8)) & 0xFFFF); 
} 
 
_Bool isDeviceExists(int result) { 
return result != -1; 
} 
 
char* getVendorName(__uint16_t vendorID) { 
  for (int i = 0; i < PCI_VENDORS_TABLE_LENGHT; i++) { 
    if (PciVenTable[i].VendorId == vendorID) { 
      return PciVenTable[i].VendorName; 
    } 
  } 
  return UNKNOWN_MESSAGE; 
} 
 
char* getDeviceName(__uint16_t vendorID, __uint16_t deviceID) { 
   for (int i = 0; i < PCI_DEVICE_TABLE_LENGHT; i++) { 
    if (PciDevTable[i].VendorId == vendorID) { 
      if (PciDevTable[i].DeviceId == deviceID) { 
        return PciDevTable[i].DeviceName; 
      } 
    } 
  }  
  return UNKNOWN_MESSAGE; 
} 
 
 
 
char getHeaderType(int reg){ 
  outl(reg+12*4, 0xCF8); 
  int result= inl(0xCFC); 
  result=result>>16; 
  result=result&0xFF; 
  return result%2; 
} 
 
 
typedef struct { 
  int bus; 
  int device; 
  int function; 
  __uint16_t vendorID; 
  __uint16_t deviceID; 
  char* vendorName; 
  char* deviceName; 
} Device; 
 
Device createDevice(int bus, int device, int function, int data) { 
  __uint16_t vendorID = readPCIDataField(data, VENDOR_ID_OFFSET); 
  __uint16_t deviceID = readPCIDataField(data, DEVICE_ID_OFFSET); 
  char* vendorName = getVendorName(vendorID); 
  char* deviceName = getDeviceName(vendorID, deviceID); 
  Device temp = {bus, device, function, vendorID, deviceID, vendorName, deviceName};  
  return temp; 
} 
 
void printDevice(Device device) { 
printf("%02x:%02x.%1x %04x %04x %s %s ", device.bus, device.device, device.function, device.vendorID, device.deviceID, device.vendorName, device.deviceName); 
} 
 
void printChacheInp(int addr){ 
  outl(addr+0xC*4, 0xCF8); 
  int Chache= inl(0xCFC); 
  Chache=Chache&0xFF; 
  printf(" \n Chache: %d;", Chache); 
  int count=0; 
  int regVal; 
  while(count<6){ 
    outl(addr+(0x10+4*count)*4, 0xCF8); 
    int regVal= inl(0xCFC); 
    if(regVal%2==1 ){ 
      regVal=regVal&0xFFFFFFFC; 
      regVal=regVal>>2; 
      int CountZeros=2; 
      for(int i=0; i<20; i++){ 
         if(regVal%2==0){ 
           CountZeros++; 
         } 
         regVal=regVal/2; 
      } 
      int MemReq=1; 
      for(int i=0; i<CountZeros; i++){ 
        MemReq*=2; 
      } 
      regVal=regVal<<22; 
      printf(" \n Mem required %d;", MemReq); 
      printf(" \n Addr %u", regVal); 
 
   } 
   count++; 
 } 
 
} 



 
void printMem(int addr){ 
   outl(addr+0x20*4, 0xCF8); 
   int Mem= inl(0xCFC); 
   printf("\n Mem base: %d, \n Mem lim: %d", Mem&0xFFFF, (Mem>>16)&0xFFFF); 
}
 
int main(void) { 
  if (iopl(PRIVILEGE_LEVEL)) { 
    printf("I/O privilege level change error: try running under root user! \n"); 
    return 1; 
  } 
 
  for (int busIndex = 0; busIndex < BUS_NUM; busIndex++) { 
    for (int deviceIndex = 0; deviceIndex < DEVICE_NUM; deviceIndex++) { 
      for (int functionIndex = 0; functionIndex < FUNCTION_NUM; functionIndex++) { 
        int addr=ENABLE_BIT + (busIndex <<16) + (deviceIndex << 11) + 
                 (functionIndex << 8); 
        outl(addr,0xCF8); 
        int data= inl(0xCFC); 
        if (isDeviceExists(data)) { 
           Device device = createDevice(busIndex, deviceIndex, functionIndex, data); 
           printDevice(device); 
           char isBus=getHeaderType(addr); 
           if(!isBus){ 
              printChacheInp(addr); 
           } 
           else printMem(addr); 
           printf("\n"); 
  
        } 
      } 
    } 
  } 
  return 0; 
}
