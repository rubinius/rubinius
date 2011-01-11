---
layout: doc_ru
title: Запуск Rubinius
previous: Сборка
previous_url: getting-started/building
next: Разрешение проблем
next_url: getting-started/troubleshooting
---

После того, как вы прошли шаги по сборке (и, возможно, установке)
Rubinius, можно проверить его работоспособность:

    rbx -v

Как правило, Rubinius работает из командной строки так же как и Ruby. Пример:

    rbx -e 'puts "Hello!"'

Для запуска ruby файла с именем 'code.rb':

    rbx code.rb

Для запуска IRB:

    rbx

Если вы добавили директорию bin Rubinius в вашу переменную окружения PATH,
Rubinius будет себя вести точно также, как MRI. Будут доступны программы
`ruby`, `rake`, `gem`, `irb`, `ri` и `rdoc`.

Добавляйте директорию bin Rubinius в PATH только тогда, когда вы захотите
перейти на использование Rubinius. В противном случае он будет использоваться
вместо вашего стандартного Ruby интерпретатора.
