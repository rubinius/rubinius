---
layout: doc_it
title: Requisiti
previous: Primi passi
previous_url: getting-started
next: Compilare Rubinius
next_url: getting-started/building
---

Assicuratevi di aver installato i programmi e le librerie che seguono.
Si vedano inoltre i paragrafi successivi per i requisiti particolari del
vostro sistema operativo.

Quelli che seguono sono suggerimenti per ottenere maggiori informazioni sui
programmi e le librerie necessari per compilare Rubinius. Il vostro sistema
operativo o il vostro gestore di pacchetti potrebbero avere altri pacchetti
disponibili.

  * [GCC e G++ 4.x](http://gcc.gnu.org/)
  * [GNU Bison](http://www.gnu.org/software/bison/)
  * [MRI Ruby 1.8.7+](http://www.ruby-lang.org/) Se sul vostro sistema non è
    installato Ruby 1.8.7, considerate l'uso di
    [RVM](http://rvm.beginrescueend.com/) per installarlo.
  * [Rubygems](http://www.rubygems.org/)
  * [Git](http://git.or.cz/)
  * [ZLib](http://www.zlib.net/)
  * pthread - La libreria pthread dovrebbe essere stata installata come parte 
    del vostro sistema operativo
  * [gmake](http://savannah.gnu.org/projects/make/)
  * [rake](http://rake.rubyforge.org/) `[sudo] gem install rake`


### Apple OS X

Il modo più facile per ottenere un ambiente di compilazione su Apple OS X è
installare gli Strumenti di sviluppo XCode. Una volta installati, potete 
abilitare la modalità di visualizzazione degli errori "Sviluppatore"
lanciando l'applicazione
/Developer/Applications/Utilities/CrashReporterPrefs.app


### Debian/Ubuntu

  * ruby-dev o ruby1.8-dev
  * libreadline5-dev
  * zlib1g-dev
  * libssl-dev

### FreeBSD

Rubinius ha un port nell'albero dei port di FreeBSD. Si chiama
`lang/rubinius`. Potete trovare informazioni sul port all'indirizzo
<http://freshports.org>. Una volta installato il port installa tutte le
dipendenze automagicamente.
