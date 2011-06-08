---
layout: doc_ru
title: How-To - Написание Ruby спецификации
previous: Создание тикета
previous_url: how-to/write-a-ticket
next: Исправление неработающей спецификации
next_url: how-to/fix-a-failing-spec
---

Убедитесь, что вы прочитали следующие разделы:

  *  [Приступая к работе](/doc/ru/getting-started/)
  *  [Спецификации](/doc/ru/specs/)

После этого, чтобы написать спецификацию для Ruby метода проделайте следующее:

  1. Отредактируйте файл в `spec/ruby/...`
  2. Выполните `bin/mspec -tr spec/ruby/some/spec_file.rb`
  3. Повторяйте до тех пор, пока спецификация не пройдет на MatzRuby
  4. Закоммитьте изменения
  7. Используйте `git format-patch`
  8. Создайте gist с вашим патчем и добавьте ссылку на него в тикет в трекере
     https://github.com/rubinius/rubinius/issues.
