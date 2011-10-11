---
layout: doc_ru
title: Перевод документации
previous: Написание Документации
previous_url: how-to/write-documentation
next: Коммиты в Github
next_url: how-to/commit-to-github
---

У переводчика две задачи:

1. обновление существующего перевода некоторой статьи;
1. добавление нового перевода.

Начните с чтения главы [<<Написание
документации>>](/doc/ru/how-to/write-documentation/)


### Обновление существующего перевода

Чтобы обновить существующий перевод, откройте нужный файл статьи в директории
`web/doc/LANG` и отредактируйте существующий текст или добавьте
свежепереведенный.


### Добавление нового перевода

Чтобы создать новую языковую версию перевода, нужно:

1. Скопировать `web/doc/en` в `web/doc/LANG`, где _LANG_ --- это
   код стандарта [ISO-639-2](http://en.wikipedia.org/wiki/List_of_ISO_639-2_codes) для
   языка, на который делается перевод.
1. Отредактировать ссылки в <<Содержании>>, чтобы они указывали на правильные файлы
   перевода. (**Внимание:** по некоторым причинам атрибут `page.base_dir` недоступен, когда
   файлы обрабатываются программой Jekyll. С этим моментом стоит разобраться.)
1. Переведите английский текст на выбранный вами язык.
1. Отредактируйте файл `web/_layouts/doc.html`: добавьте необходимые значения
   в атрибут yaml-шапки `languages`.
1. Добавьте необходимые значения в массив `TRANS` в файле `rakelib/web.rake`
   (см. ниже).

### Инструменты переводчика

Мы создали крошечные программки для облегчения жизни переводчиков.
Точнее, формально программка пока только одна, и называется она **<<Проверка
соответствия переводов>>**.

#### Краткий пример использования

    $ cd rubinius
    $ rake -T
    
    ...

    rake web:translations:check                # Check web documentation translations for existence and mtime consistency.

    $ rake web:translations:check
    (in /usr/home/paul/develop/ruby/rubinius)
    
    TRANSLATION CHECK: Checking 'web/doc/en' recursively against ["de", "es", "fr", "ja", "pl", "pt-br", "ru"] versions
    
    NB:
    
      LOST IN TRANSLATION:
        web/doc/en/how-to/write-benchmarks.markdown: doesn't exist in 'ru' edition
        web/doc/en/bytecode-compiler/customization.markdown: doesn't exist in 'ru' edition
        web/doc/en/bytecode-compiler/encoder.markdown: doesn't exist in 'ru' edition
        web/doc/en/bytecode-compiler/packager.markdown: doesn't exist in 'ru' edition
        web/doc/en/bytecode-compiler/writer.markdown: doesn't exist in 'ru' edition
    
    
      LAG IN TRANSLATION:
        web/doc/en/garbage-collector/large-objects.markdown: is younger than its 'pl' version
        web/doc/en/garbage-collector/mature-generation.markdown: is younger than its 'ru' version
        web/doc/en/garbage-collector/nursery.markdown: is younger than its 'ru' version
        web/doc/en/garbage-collector/young-generation.markdown: is younger than its 'ru' version
        web/doc/en/systems/concurrency.markdown: is younger than its 'pl' version
        web/doc/en/ruby/instance-variables.markdown: is younger than its 'ru' version
        web/doc/en/bytecode-compiler/ast.markdown: is younger than its 'ru' version
        web/doc/en/bytecode-compiler/parser.markdown: is younger than its 'pl' version
        web/doc/en/garbage-collector.markdown: is younger than its 'ru' version

#### Пояснения

Существует rake-задача `web:translations:check`, которая рекурсивно сверяет
лидер-версию (сейчас это `web/doc/en`) с файлами переводов по соответствующему
списку. Лидер-версия и список переводов определены в `rakelib/web.rake`:

    # This is the 'leader' version: all is translated from here.
    LEADER = "web/doc/en"

    # Here we MANUALLY list all official doc translations
    TRANS  = [
      'de',
      'es',
      'fr',
      'ja',
      'pl',
      'pt-br',
      'ru'
    ]

Для каждого лидер-файла скрипт проверяет:

  * есть ли вообще соответствующий файл перевода;
  * если есть, то не старше ли он своей лидер-версии (сравниваются `mtime`
    файлов)?

Если перевод старше оригинала или вообще отсутствует, Вы увидите то ужасно
красивое сообщение, которое приведено в нашем примере. Если с файлами все в
порядке, программа сообщит и об этом --- Вы заметите.

#### Заключение

Эта супер-нано-пупер-тупая тулза была написана для того, чтобы напомнить
переводчику, какие именно файлы могут нуждаться в немедленной доработке.
Никакого интеллектуального анализа, например, на валидность перевода и т.п.
пока не планируется. Если Вы --- умнейший контрибутор Rubinius всех времен и
народов, мы приглашаем Вас расширить возможности настоящего инструмента,
помочь обществу и спасти мир!
