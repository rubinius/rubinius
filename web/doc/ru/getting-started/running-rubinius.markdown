---
layout: doc_ru
title: Запуск Rubinius
previous: Сборка
previous_url: getting-started/building
next: Разрешение Проблем
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

Если вы добавили директорию bin Rubinius в ваш PATH, вы можете ожидать
от Rubinius такого же поведения, как от MRI. Будут доступны команды
`ruby`, `rake`, `gem`, `irb`, `ri`, and `rdoc`.

Можно добавить директорию bin Rubinius в PATH только тогда, когда вы захотите
использовать Rubinius. Так он не будет мешать вашей обычной версии
Ruby в случаях, когда использовать Rubinius не нужно.
