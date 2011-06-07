---
layout: doc_pl
title: Wymagania
previous: Pierwsze kroki
previous_url: getting-started
next: Kompilacja
next_url: getting-started/building
---

Upewnij się, że masz zainstalowane poniższe programy i biblioteki
przed instalacją Rubiniusa. Poza tym przejrzyj podsekcje poniżej,
które opisują dokładne wymagania dla danego systemu operacyjnego.

Poniżej zostały przedstawione sugestie co do programów oraz bibliotek
wymaganych do skompilowania Rubiniusa. Twój system operacyjny lub
system zarządzania pakietami może udostępniać inne odpowiedniki tych bibliotek.

  * [GCC oraz G++ 4.x](http://gcc.gnu.org/)
  * [GNU Bison](http://www.gnu.org/software/bison/)
  * [MRI Ruby 1.8.7+](http://www.ruby-lang.org/) Jeśli nie masz
    zainstalowanego Ruby 1.8.7 w systemie rozważ skorzystanie z [RVM](https://rvm.beginrescueend.com/)
    aby go zainstalować.
  * [Rubygems](http://www.rubygems.org/)
  * [Git](http://git.or.cz/)
  * [ZLib](http://www.zlib.net/)
  * pthread - Biblioteka pthread powinna być zainstalowana przez Twój
    system operacyjny
  * [gmake](http://savannah.gnu.org/projects/make/)
  * [rake](http://rake.rubyforge.org/) `[sudo] gem install rake`


### Apple OS X

Najprostszym sposobem przygotowania środowiska w systemie Apple OS X
jest zainstalowanie narzędzi XCode. Po zainstalowaniu, uaktywnij
"developer mode crash reporting": /Developer/Applications/Utilities/CrashReporterPrefs.app


### Debian/Ubuntu

  * ruby-dev lub ruby1.8-dev
  * libreadline5-dev
  * zlib1g-dev
  * libssl-dev
