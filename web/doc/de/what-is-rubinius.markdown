---
layout: doc_de
next: Erste Schritte
next_url: getting-started
---

## Was ist Rubinius?

Rubinius ist eine Implementierung der [Programmiersprache Ruby](http://ruby-lang.org).

Rubinius besteht aus einer Bytecode Virtuellen Maschine (VM), einem Ruby 
Syntaxparser, Bytecode Compiler, Generationellen Garbage Collector (GC), 
Just-In-Time (JIT) nativen Maschinencode Compiler und einer Ruby Core und 
Standard Bibliothek.

Rubinius implementiert derzeit Ruby Version 1.8.7.


## Lizenz

Rubinius nutzt die BSD Lizenz. Siehe LICENSE Datei im Quellverzeichnis.


## Installation

Rubinius läuft auf Mac OS X und vielen Unix/Linux Betriebssystemen. Microsoft 
Windows Unterstützung kommt ebenfalls bald und ist derzeit in Entwicklung.

Um Rubinius zu installieren, folge den unteren Anweisungen. Für detailliertere
Informationen siehe auch: [Erste Schritte](/doc/de/getting-started/).

1. `git clone git://github.com/rubinius/rubinius.git`
1. `cd rubinius`
1. `./configure --prefix=/pfad/zum/installations/verzeichnis`
1. `rake install`

Wenn der Installationsvorgang abgeschlossen ist, folge den Anweisungen, um das
Rubinius Binärdateiverzeichnis (bin) deinem PATH hinzuzufügen.

Rubinius kommt mit eingebautem RubyGems und hat die rake und rdoc Gems von Haus
aus vorinstalliert. Um zum Beispiel das nokogiri Gem zu installieren, führe 
`rbx gem install nokogiri` aus.
