---
layout: doc_ru
title: How-To - Написание Ruby спецификации
previous: Создание тикета
previous_url: how-to/write-a-ticket
next: Исправление неработающей спецификации
next_url: how-to/fix-a-failing-spec
---

Make sure you have read:

  *  [Приступая к работе](/doc/ru/getting-started/)
  *  [Спецификации](/doc/ru/specs/)

Then, follow these steps to write a spec for a Ruby method:

  1. Edit a file under `spec/ruby/...`
  2. Run `bin/mspec -tr spec/ruby/some/spec_file.rb`
  3. Repeat until the spec passes on MatzRuby
  4. Commit your changes
  7. Use `git format-patch`
  8. Create a gist with your patch and link to it in a ticket on the issue
     tracker at http://github.com/evanphx/rubinius/issues.
