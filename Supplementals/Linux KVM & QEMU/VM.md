2025.02.

Дано: установлена свежая и актуальная версия Ubuntu.

Нужно: установить виртуальную машину Windows 10 для запуска специфичного ПО.

Обеспечить возможность передачи файлов с хоста на VM и обратно.

Используем KVM/QEMU.

Писалось не в процесса, а по итогам.

Вывод: разворачивание и настройка VM под Linux делается быстро (1-2 часа) стандартными средствами. Возможно, с VirtualBox было бы еще проще. Почему-то достаточно полной инструкции не нашлось.

1) Установка ПО виртуализации KVM/QEMU.

Выполняем команды установки пакетов.

sudo apt-get update

sudo apt install -y qemu-kvm libvirt-clients libvirt-daemon-system bridge-
utils virt-manager

sudo apt install -y virt-viewer

sudo apt install -y spice-vdagent ovmf

sudo apt install -y swtpm swtpm-tools

sudo apt install -y qemu-guest-agent

См. также

https://nmanzi.com/blog/windows-guest-on-linux-mint/

https://www.nic.ru/help/kak-ustanovit6-i-nastroit6-kvmqemu_11630.html

2) Создание VM и установка ОС.

После установки пакетов появляется приложение
Virtual Machine Manager.

В нем, в соответствии с инструкциями найденными в Google, 
используя мышку создается VM с нужными параметрами CPU, Memory, HDD.
Используется установочный CD (iso файл) Windows 10.
Пункт меню New Virtual Machine.

См. также
https://nmanzi.com/blog/windows-guest-on-linux-mint/

Примеры интерфейса:

![Пример 1](https://github.com/VeretennikovAlexander/OS-Public/blob/master/Supplementals/Linux%20KVM%20%26%20QEMU/Create%20a%20new%20virtual%20machine%201.png) 

![Пример 2](https://github.com/VeretennikovAlexander/OS-Public/blob/master/Supplementals/Linux%20KVM%20%26%20QEMU/Create%20a%20new%20virtual%20machine%202.png) 

![Пример 3](https://github.com/VeretennikovAlexander/OS-Public/blob/master/Supplementals/Linux%20KVM%20%26%20QEMU/Create%20a%20new%20virtual%20machine%203.png) 

![Пример 4](https://github.com/VeretennikovAlexander/OS-Public/blob/master/Supplementals/Linux%20KVM%20%26%20QEMU/Create%20a%20new%20virtual%20machine%204.png) 
 
![Пример 5](https://github.com/VeretennikovAlexander/OS-Public/blob/master/Supplementals/Linux%20KVM%20%26%20QEMU/Create%20a%20new%20virtual%20machine%205.png) 

![Пример 6](https://github.com/VeretennikovAlexander/OS-Public/blob/master/Supplementals/Linux%20KVM%20%26%20QEMU/Create%20a%20new%20virtual%20machine%206.png) 

3) Установка антивируса и базового ПО (типа браузера Chrome), а также
последних апдейтов Windows.

4) Настройка автовыключения VM при выключении хоста.

sudo mcedit /etc/default/libvirt-guests

Настройки:

ON_SHUTDOWN=shutdown

SHUTDOWN_TIMEOUT=300

См. также

https://blog.wirelessmoves.com/2022/08/proper-shutdown-of-vms-on-host-reboot.html

В целом тема автоматического запуска VM
при старте хоста и выключения VM при выключении хоста (по сути VM как сервис) имеет свои особенности.

P.S. Настройки сделаны, но пока не проверялись.

5) Установка драйверов virtio-win-0.1.266.iso на VM.

Скачиваем:

https://fedorapeople.org/groups/virt/virtio-win/direct-downloads/stable-virtio/virtio-win.iso

Монтируем CD в VM (в интерфейсе Virtual Machine Manager) и запускаем установку драйверов из корня CD.

P.S. В некоторых инструкциях говорят, что данный CD требуется подключать при
установке Windows. Здесь требуется уточнение. Посколько до данного шага дошел, когда Windows был установлен, не проверял.

6) Установка virtiofsd для обмена файлами между хостом и VM.

sudo apt install virtiofsd

В соответствии с инструкциями, добавляем устройство вида FileSystem для VM,
указываем при настройках целевую папку в файловой системе Linux.

Включаем опцию Enabled Shared Memory в Memory, в
соответствии с https://virtio-fs.gitlab.io/howto-windows.html.

На виртуалке ставим:
WinFsp - Windows File System Proxy.
После установки автоматически появился сервис VirtIO-FS Service.
Который осталось только перевести в режим запуска Automatic и запустить.
Также не потребовалось что-то делать дополнительно с драйверами в VM.

То есть, часть шагов, указанных в ссылках ниже (
https://virtio-fs.gitlab.io/howto-windows.html,
касательно драйверов и создания сервиса), не нужны.

После установки WinFsp - Windows File System Proxy и старта сервиса
автоматически появляется диск Z данные которого равны данным папки Linux.

См.

https://virtio-fs.gitlab.io/

https://virtio-fs.gitlab.io/howto-windows.html

https://sysguides.com/share-files-between-kvm-host-and-linux-guest-using-virtiofs

P.S. Альтернативный вариант, использовать сеть, но виртуальная файловая система кажется более безопасным вариантом.

7. Ограничения.

Не работает установка Office 365 от Microsoft (в рамках подписки УрФУ), похоже все таки отключили. Как тебе не стыдно Дональд Трамп?

