---
layout: doc_ru
title: Минимальные требования
previous: Для начала...
previous_url: getting-started
next: Сборка
next_url: getting-started/building
---

Убедитесь, что у вас установлены перечисленные программы и библиотеки. Также
просмотрите нижеприведенные разделы на предмет  требований для каждой
конкретной операционной системы.

Ниже следуют ссылки для получения дополнительной информации о программах и библиотеках,
нужных для сборки Rubinius. Для вашей операционной системы или менеджера
пакетов могут быть доступны и другие сборки, помимо перечисленных.

  * [GCC и G++ 4.x](http://gcc.gnu.org/)
  * [GNU Bison](http://www.gnu.org/software/bison/)
  * [MRI Ruby 1.8.7+](http://www.ruby-lang.org/) Если на вашей системе не
    установлен Ruby 1.8.7, подумайте об использовании
    [RVM](http://rvm.beginrescueend.com/) для его установки.
  * [Rubygems](http://www.rubygems.org/)
  * [Git](http://git.or.cz/)
  * [ZLib](http://www.zlib.net/)
  * pthread: Библиотека pthread должна быть установлена вашей операционной
    системой.
  * [gmake](http://savannah.gnu.org/projects/make/)
  * [rake](http://rake.rubyforge.org/): `[sudo] gem install rake`


### Mac OS X

Простейший путь для создания сборочного окружения на Mac OS X ---
установка XCode Tools and Utilities. После установки для формирования
отчетов об аварийных завершениях можно использовать приложение
/Developer/Applications/Utilities/CrashReporterPrefs.app


### Debian/Ubuntu

  * ruby-dev или ruby1.8-dev
  * libreadline5-dev
  * zlib1g-dev
  * libssl-dev

### FreeBSD

Под FreeBSD есть порт Rubinius, который называется `lang/rubinius`. Подробную
информацию о порте можно найти на сайте <http://freshports.org>. В процессе
инсталляции порта установка и настройка всех требуемых вспомогательных
программ и библиотек происходит автоматически.
