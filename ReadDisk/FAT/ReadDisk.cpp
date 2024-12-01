
#include <fstream>
#include <cerrno>
#include <stdexcept>
#include <cstring>
#include <vector>
#include <cstdint>

#pragma pack(1)

struct BPB
{
 char BS_jmpBoot[3];
 char BS_OEMName[8];
 uint16_t BPB_BytsPerSec;
 uint8_t BPB_SecPerClus;
};

#pragma options align=reset

/**
See also

Microsoft Extensible Firmware Initiative FAT32 File System Specification
FAT: General Overview of On-Disk Format
https://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/fatgen103.doc

https://vk.com/wall-227225018_64

Should be run under the superuser

*/

int main() {

  printf("Structure size %d\n", (int)sizeof(BPB));

  char diskName[] = "/dev/ram0";
  std::string diskError = std::string() + diskName + ": ";

  std::ifstream disk(diskName, std::ios_base::binary);

  if(!disk)
    throw(std::runtime_error(diskError + std::strerror(errno)));

  disk.seekg(0);
  if(!disk)
    throw(std::runtime_error(diskError + std::strerror(errno)));

  char buffer[512];
  disk.read(&buffer[0], 512);

  if(!disk)
    throw(std::runtime_error(diskError + std::strerror(errno)));

  BPB* bpb = (BPB*)buffer;

  printf("The first sector has been read\n");

  printf("%s\n", bpb->BS_OEMName);
  printf("Bytes per sector %d\n", (int)bpb->BPB_BytsPerSec);
  printf("Sectors per cluster %d\n", (int)bpb->BPB_SecPerClus);
}