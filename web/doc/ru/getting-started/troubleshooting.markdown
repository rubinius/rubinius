---
layout: doc_ru
title: Разрешение проблем
previous: Запуск Rubinius
previous_url: getting-started/running-rubinius
next: Участие в проекте
next_url: contributing
---

Ниже приведены ошибки, с которыми вы можете столкнуться в процессе сборки,
установки или использования Rubinius, вместе с возможными их решениями.

При любой ошибке, первым делом удостоверьтесь, что вы работаете с чистой
копией Rubinius. Перед тем, как двигаться дальше, попробуйте выполнить
следующее:

    $ git co master
    $ git reset --hard
    $ git pull
    $ rake distclean
    $ rake


Ошибка:

    ERROR: unable to find runtime directory

    Rubinius was configured to find the runtime directory at:

      /Users/brian/devel/rubinius/runtime

    but that directory does not exist.

    Set the environment variable RBX_RUNTIME to the location
    of the directory with the compiled Rubinius kernel files.

    You may have configured Rubinius for a different install
    directory but you have not run 'rake install' yet.

Решение:

  Если вы конфигурировали rubinius с --prefix, выполните rake install.

  Если вы конфигурировали rubinius с --prefix и переименовали инсталяционную
  директорию после установки Rubinius, выполните конфигурацию и установку
  заново.

  Если вы переименовали директорию c исходниками после сборки Rubinius,
  выполните конфигурацию и установку вновь.

  Вообще, лучше не переименовывать директорию с исходниками или директорию 
  где производилась сборка после сборки Rubinius.
