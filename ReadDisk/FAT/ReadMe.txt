Задача (Linux):
1) Создать диск с файловой системой FAT.
2) В корневом каталоге создать файл 1.txt.
3) Используя прямой доступ к диску (в обход драйверов файловой системы)
прочитать файл и вывести на экран его содержимое.

Подробнее:
1-2) Для создания диска с файловой системой FAT
используем RamDisk.
См. CreateDisk.sh

Создание RamDisk-а (/dev/ram0) объемом 4 ГБ:
modprobe brd rd_nr=1 rd_size=$((4 * 1048576))

Форматирование созданного диска /dev/ram0 с использованием FAT32:
mkfs -t vfat -F 32 /dev/ram0

Создание папки в структуре каталогов:
mkdir /opt/temp
mkdir /opt/temp/cpp
mkdir /opt/temp/cpp/fat32

Монтирование созданного диска в заданную папку:
mount /dev/ram0 /opt/temp/cpp/fat32

Создание тестового файла
echo Test > /opt/temp/cpp/fat32/1.txt

Вывод информации о файловых системах:
df -h -T

Команды выполняются под root.

3) См.

ReadDisk.cpp
ReadDisk.sh

Компиляция при помощи clang:

clang++ -std=c++23 ReadDisk.cpp -o ReadDisk

Компиляция при помощи g++:

g++ -Wall ReadDisk.cpp -o ReadDisk

Запуск программы под root: ./ReadDisk

В начальной версии программы осуществляется чтение первого сектора
с виртуального диска /dev/ram0.

Далее выводятся значения нескольких полей заголовка FAT (BPB).

См. далее

Спецификация FAT:
Microsoft Extensible Firmware Initiative FAT32 File System Specification
FAT: General Overview of On-Disk Format
https://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/fatgen103.doc

https://vk.com/wall-227225018_64

https://vk.com/wall-227225018_73

https://codeby.net/threads/asm-fat32-opisanie-fajlovoj-sistemy.79019/

Примечание.

Для работы с C++ можно установить IDE, например Geany:

apt-get install geany
