---
layout: doc_ru
title: Минимальные требования
previous: Приступая к работе
previous_url: getting-started
next: Сборка
next_url: getting-started/building
---

Убедитесь, что у вас установлены следующие программы и библиотеки. Также
посмотрите разделы ниже с требованиями для каждой конкретной операционной
системы.

Далее следуют ссылки для получения информации о программах и библиотеках,
нужных для сборки Rubinius. Для вашей операционной системы или пакетного
менеджера могут быть доступны другие пакеты.

  * [GCC и G++ 4.x](http://gcc.gnu.org/)
  * [GNU Bison](http://www.gnu.org/software/bison/)
  * [MRI Ruby 1.8.7+](http://www.ruby-lang.org/) Если на вашей системе не
    установлен Ruby 1.8.7, подумайте об использовании
    [RVM](http://rvm.beginrescueend.com/) для установки.
  * [Rubygems](http://www.rubygems.org/)
  * [Git](http://git.or.cz/)
  * [ZLib](http://www.zlib.net/)
  * pthread - Библиотека pthread должна быть установлена вашей операционной системой
  * [gmake](http://savannah.gnu.org/projects/make/)
  * [rake](http://rake.rubyforge.org/) `[sudo] gem install rake`


### Apple OS X

Простейший путь для создания сборочного окружения на Apple OS X —
установка XCode Tools and Utilities. После установки, для формирования
отчетов об аварийных завершениях можно использовать приложение
/Developer/Applications/Utilities/CrashReporterPrefs.app


### Debian/Ubuntu

  * ruby-dev или ruby1.8-dev
  * libreadline5-dev
  * zlib1g-dev
  * libssl-dev
