---
layout: doc_ru
title: Компилятор в байткод
previous: Custom Dispatch Logic
previous_url: virtual-machine/custom-dispatch-logic
next: Парсер
next_url: bytecode-compiler/parser
review: true
---

Байткод-компилятор Rubinius превращает исходный код на Ruby в байткод,
исполняемый в виртуальной машине. В процессе трансформации входного языка в
код, понятный виртуальной машине, байткод-компилятор выполняет серию
последовательных операций.

Каждая из этих операций-стадий выполняется независимо от всего остального процесса,
ожидая определенного материала на входе и передавая свой вывод следующей
стадии. В результате этого весь процесс компиляции получается довольно
конфигурабельным, а каждую отдельную стадию можно достаточно просто
перенастроить.

Итак, каждая стадия получает ввод, выполняется и передает вывод следующей
стадии. Ниже проиллюстрированы стандартные стадии вместе со своими исходными и
выведенными данными.

<div style="text-align: center; width: 100%">
  <img src="/images/compilation_process.png" alt="Compilation process" />
</div>

1. [Парсер](/doc/ru/bytecode-compiler/parser/)
1. [AST](/doc/ru/bytecode-compiler/ast/)
1. [Генератор](/doc/ru/bytecode-compiler/generator/)
1. [Кодировщик](/doc/en/bytecode-compiler/encoder/)
1. [Упаковщик](/doc/en/bytecode-compiler/packager/)
1. [Запись](/doc/en/bytecode-compiler/writer/)
1. Принтеры
1. [Преобразования](/doc/ru/bytecode-compiler/transformations/)
1. [Настройка конвейера](/doc/ru/bytecode-compiler/generator/)
