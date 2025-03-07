import platform
import os
import wmi
   
print("Physcal discs and their partitions")
print("")

'''
https://learn.microsoft.com/en-us/windows/win32/cimwin32prov/win32-diskdrive

keywords: WMI Win32_DiskDrive, WMI Win32_LogicalDisk

https://github.com/tjguk/wmi
'''

def showDisk(mediaType, typeStr):
    
    disks = wmi.WMI().Win32_DiskDrive(MediaType=mediaType)
    for disk in disks:
        diskSize = int(disk.size)
        mediaType = disk.MediaType
        sectorSize = disk.BytesPerSector
        print(disk.name, "media %s, disk size %d" % (mediaType, diskSize))
        
        for partition in disk.associators("Win32_DiskDriveToDiskPartition"):
            for logicalDisk in partition.associators("Win32_LogicalDiskToPartition"):
                print(disk.Caption, partition.Caption, logicalDisk.Caption, logicalDisk.volumeName)
        print("")
            
        
showDisk("Fixed hard disk media", "Fixed")
showDisk("External hard disk media", "External")

print("Local disks")
print("")

for disk in wmi.WMI().Win32_LogicalDisk(DriveType=3):
    print("%s; %s; %s; %s, %s, %s, %s,%s, %s, %s, %s" % (disk.Caption, 
        disk.Description, disk.ProviderName or "", disk.DriveType, disk.Name, disk.volumeName, disk.description,
        disk.SystemName, disk.MediaType, disk.InstallDate, disk.DeviceID))


