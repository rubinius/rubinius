---
layout: doc_de
title: Abhängigkeiten
previous: Erste Schritte
previous_url: getting-started
next: Rubinius bauen
next_url: getting-started/building
---

Es sollte sichergestellt sein, dass die folgenden Programme und
Bibliotheken installiert sind. Außerdem sollten auch die
betriebssystemabhängigen Voraussetzungen (in den Unterabschnitten
beschrieben) erfüllt sein.

Im Folgenden eine Auflistung an vorgeschlagenen Links um weitere
Informationen über die Programme und Bibliotheken zu erhalten, die
benötigt werden um Rubinius erfolgreich zu bauen. Ggf. bietet dein
Betriebssystem oder Paketmanager andere/weitere Pakete bereit.

  * [GCC and G++ 4.x](http://gcc.gnu.org/)
  * [GNU Bison](http://www.gnu.org/software/bison/)
  * [MRI Ruby 1.8.7+](http://www.ruby-lang.org/) Falls Ruby 1.8.7
    nicht auf deinem System installiert ist, sollte
    [RVM](http://rvm.beginrescueend.com/) in Erwägung gezogen werden.
  * [Rubygems](http://www.rubygems.org/)
  * [Git](http://git.or.cz/)
  * [ZLib](http://www.zlib.net/)
  * pthread - Die pthread Bibliothek sollte auf deinem System installiert sein.
  * [gmake](http://savannah.gnu.org/projects/make/)
  * [rake](http://rake.rubyforge.org/) `[sudo] gem install rake`


### Apple OS X

Der einfachste Weg um eine Bauumgebung auf Apple OS X einzurichten ist
die Installation der XCode Tools und Utilities. Sobald diese
installiert sind kannst du developer mode crash reporting hier
aktivieren: /Developer/Applications/Utilities/CrashReporterPrefs.app


### Debian/Ubuntu

Es gibt folgende Pakete die installiert werden sollten:

  * ruby-dev oder ruby1.8-dev
  * libreadline5-dev
  * zlib1g-dev
  * libssl-dev
