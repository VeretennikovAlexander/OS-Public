Geany -- компактная IDE, подходящая для решения учебных задач на C++.

Установка:

apt-get install geany

См. также

https://www.geany.org/download/third-party/

Установка опции компилятора

g++ -std=c++2b

в Geany.

Требуется для работы с counting_semaphore (C++20).

1) Запустить окно Set Build Commands.

2) Поменять настройки Command в Compile и Build.

Применить опцию "-std=c++2b".

См. также
https://en.cppreference.com/w/cpp/thread/counting_semaphore
