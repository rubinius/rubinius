---
layout: doc_ru
title: Запуск Rubinius
previous: Сборка
previous_url: getting-started/building
next: Решение проблем
next_url: getting-started/troubleshooting
---

После того, как Вы прошли сборку (и, возможно, установку)
Rubinius, можно проверить его работоспособность:

    rbx -v

Как правило, Rubinius работает из командной строки так же как и Ruby. Пример:

    rbx -e 'puts "Hello!"'

Для запуска ruby-файла с именем 'code.rb':

    rbx code.rb

Для запуска IRB:

    rbx

Если вы добавили директорию bin Rubinius в вашу переменную окружения PATH,
Rubinius будет себя вести точно также, как MRI. Будут доступны программы
`ruby`, `rake`, `gem`, `irb`, `ri` и `rdoc`.

Добавляйте директорию `bin` Rubinius в PATH только тогда, когда Вы захотите
перейти на постоянное использование Rubinius. Иначе он будет использоваться
вместо вашего обычного Ruby-интерпретатора.
