---
layout: doc_it
next: Primi passi
next_url: getting-started
---

## Cos'è Rubinius

Rubinius è un'implementazione del [Linguaggio di programmazione
Ruby](http://ruby-lang.org).

Rubinius comprende una macchina virtuale per il bytecode, un parser della
sintassi Ruby, un compilatore di bytecode, un garbage collector generazionale,
un compilatore di linguaggio macchina nativo just-in-time (JIT), e le librerie
Core e Standard di Ruby.

Rubinius al momento implementa la versione 1.8.7 di Ruby.


## Licenza

Rubinius utilizza la licenza BSD. Si veda il file LICENSE nei sorgenti.


## Installazione

Rubinius funziona su Mac OS X e molti sistemi operativi Unix/Linux. Presto
sarà disponibile il supporto a Microsoft Windows.

Per installare Rubinius, eseguire i seguenti passi. Per informazioni più 
dettagliate, si veda [Primi passi](/doc/en/getting-started/).

1. `git clone git://github.com/evanphx/rubinius.git`
1. `cd rubinius`
1. `./configure --prefix=/path/to/install/dir`
1. `rake install`

Terminata l'installazione, seguite le istruzioni per aggiungere la cartella
degli eseguibili di Rubinius (bin) al vostro PATH.

Rubinius comprende RubyGems e le gemme rake and rdoc sono pre-installate.
Per installare la gemma nokogiri, ad esempio, eseguite `rbx gem install
nokogiri`.
