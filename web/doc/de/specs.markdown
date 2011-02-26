---
layout: doc_de
title: Specs
previous: Ruby - Global Variables
previous_url: ruby/global-variables
next: RubySpec
next_url: specs/rubyspec
---

Im Allgemeinen verwendet das Rubinius Projekt einen TDD/BDD bezogenen Stil, um
die weitere Entwicklung voranzubringen. Das Rubinius 'spec' Verzeichnis ist
konzeptuell in zwei Teile aufgeteilt:

  1. Alle Dateien, die sich im Verzeichnis './spec/ruby' befinden, beschreiben
     das Verhalten von MatzRuby.
  2. Und alle anderen Dateien, die sich im './spec' Verzeichnis befinden, 
     beschreiben das Verhalten von Rubinius.

Die Specs unter ./spec/ruby sind eine Kopie von RubySpec und werden regelmäßig
mit dem RubySpec Projekt abgeglichen, damit erfolglose Specs direkt 
markiert werden können. Auf diese Art und Weise kann gewährleistet werden, 
dass der CI Prozess immer auf einem sicher laufenden Set von Specs 
funktioniert. Ebenso kann so sichergestellt werden, dass Veränderungen am 
Rubinius Code keine Regressionen verursacht.

Dokumentation zur Organisation der Specs und Richtlinien zum Schreiben von 
Specs befinden sich beim [RubySpec Projekt](http://rubyspec.org/).

Folgender Arbeitsablauf sollte beim Hinzufügen von Specs und Code für Rubinius 
verwendet werden:

  1. Schreibe Failing Specs, um das Verhalten von Ruby an einigen Stellen zu 
     beschreiben. Verwende separate Commits für die unter ./spec/ruby 
     angepassten zuständigen Dateien.
  2. Füge den Rubinius Code hinzu, der die Specs bestehen lässt. Ebenso sollten
     diese Änderungen als eigenständige Commits, unabhängig von den Specs,
     übergeben werden.
  3. Lass `rake` laufen, um zu prüfen, dass alle CI Specs bestehen.
  
Die Veränderungen unter ./spec/ruby werden regelmäßig an das RubySpec Projekt
weitergeleitet. Ebenso werden aktuelle Änderungen an RubySpec durch Beiträge
anderer Ruby Implementationen übernommen. Darüber hinaus werden die CI Tags bei
Updates von RubySpec auf den neusten Stand gebracht.
