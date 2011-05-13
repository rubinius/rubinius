---
layout: doc_ru
title: Решение проблем
previous: Запуск Rubinius
previous_url: getting-started/running-rubinius
next: Участие в проекте
next_url: contributing
---

Ниже приведены ошибки, с которыми Вы можете столкнуться в процессе сборки,
установки или использования Rubinius, и их возможные решения.

При любой ошибке первым делом удостоверьтесь, что Вы работаете с последней чистой
версией Rubinius. Перед тем, как двигаться дальше, попробуйте выполнить
следующее:

    $ git co master
    $ git reset --hard
    $ git pull
    $ rake distclean
    $ rake


### Rubinius не может найти директорию runtime-библиотек

    ERROR: unable to find runtime directory

    Rubinius was configured to find the runtime directory at:

      /Users/brian/devel/rubinius/runtime

    but that directory does not exist.

    Set the environment variable RBX_RUNTIME to the location
    of the directory with the compiled Rubinius kernel files.

    You may have configured Rubinius for a different install
    directory but you have not run 'rake install' yet.

#### Решение

  Если Вы конфигурировали rubinius с `--prefix`, выполните `rake install`.

  Если Вы конфигурировали rubinius с `--prefix` и переименовали инсталяционную
  директорию после установки Rubinius, выполните конфигурацию и установку
  заново.

  Если Вы переименовали директорию c исходниками после сборки Rubinius,
  выполните конфигурацию и установку вновь.

  Вообще, лучше не переименовывать директорию с исходниками или директорию 
  инсталляции после сборки Rubinius.

### Rubinius, собранный под FreeBSD, падает по `segfault`

  До версии 8.1-STABLE включительно во FreeBSD существует проблема с
  библиотекой `execinfo`, приводящая к падению Rubinius при запуске.

#### Решение

  На стадии `configure` отключите `execinfo`:

    ./configure --without-execinfo
