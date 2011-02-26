---
layout: doc_de
title: Stilvorlage
previous: Kommunikation
previous_url: contributing/communication
next: Ruby
next_url: ruby
---

Die folgenden Richtlinien haben das Ziel den Rubinius Code möglichst 
wartungsarm zu halten. Sollte es zu einer der Richtlinien Fragen geben, 
frag am besten auf dem #rubinius IRC Kanal auf irc.freenode.net nach.


## Grundsätzliches

  * Stellen deinen Editor auf soft Tabs ein, nicht hard Tabs
  * Tabs sollen in zwei Leerzeichen umgewandelt werden
  * Füge an das Ende jeder Datei eine Leerzeile ein

## C++ Code
  * Kein Leerzeichen zwischen Bedingung und Klammern
      Schreibe `if(1)` NICHT `if (1)`

  * Schreibe die öffnende Klammer auf die gleiche Zeile wie die 
    Funktionsdeklaration oder die Bedingung

  * Verwende immer geschweifte Klammern, auch wenn man sie weglassen kann.

  * Schreibe Klammern zur Darstellung von Vorrang (soweit möglich)

  * Alternative Versionen von Funktionen sollten mit einer Erklärung zur
    Abweichung benannt werden. Gibt es z.B. eine Funktion 'person()' und du
    möchtest eine Funktion erstellen, die den Namen der Person annimmt, dann
    solltest du sie 'person_with_name(char \*name)' oder 
    'person_with_details(char \*name, ...)' nennen. 
    Nicht jedoch 'person1(char \*name)'.

## Ruby Code
  
  * Methoden: Versuche deine Methoden kurz zu halten--eine Bildschirmlänge
    als Anhaltspunkt und verwende DRY in sinnvollem Maße. Üblicherweise 
    sollten häufig verwendete Funktionen abstrahiert und in Helfer-Methoden 
    gepackt werden (die dann auch 'private' sein können). In manchen Fällen 
    macht es keinen Sinn sich an DRY zu halten, z.B. wenn es um die Core 
    Klassen geht. In diesen Fällen macht DRY alles nur unnötig kompliziert und
    man muss auf zu viele Spezialfälle in der Fehlerbehandlung eingehen.

  * Methodennamen sollten eindeutig, ausdrucksvoll, verständlich (und damit 
    auf Englisch) sein. Verwende, bis auf wenige Ausnahmen, keine 
    Unterstriche, um Methoden zu schützen ('\_\_send\_\_').

  * Smalltalkartige Methodennamen sind ok, d.h. du kannst Methoden mit 
    `SomeClass.make_from` benennen, wenn damit `SomeClass.make_from file` oder 
    `SomeClass.make_from :file => name` beabsichtig ist. Dabei 
    _vervollständigt_ der Parameter `name` den Methodennamen
    und ermöglicht ein natürliches Lesen des Codes.
    
  * Variablennamen: Wähle eindeutige und ausdrucksstarke Namen (zu den üblichen
    Ausnahmen gehören z.B. Zählervariablen wie `i`). Vermeide es Methodennamen
    für Variablen zu verwenden. Üblich sind 'klass' an Stelle von 'class' oder
    'idx' innerhalb von Arrays, da 'index' bereits als Methodenname existiert.

  * Verwende nachgefügte 'if' und 'unless' *nur* wenn deine Ausdrücke in eine
    Zeile passen *und* es nicht zu viele Bedingungen gibt.

  * Blöcke: Verwende entweder `do ... end` oder `{...}` mit Leerzeichen 
    zwischen den Trennzeichen und dem Programmcode (`foo { |arg| code }`).
    Verteile lange oder komplexe Ausdrücke über mehrere Zeilen. Beispiel:

        mapped = foo.map do |elem|
          do_something_with elem
        end

  * Modul- und Klassendefinitionen werden mit Gültigkeitsbereich (Scope) 
    versehen:
  
        module Rubinius::Profiler
          class Sampler
          end
        end

## Kernel Code

Die erste Richtlinie für den kompletten Kernel Code ist einfach und 
leistungsstark zu sein. Einfach geschriebener Code ist häufig effizienter und 
meist auch leichter nachzuvollziehen. Im Bootstrap Bereich sollte keine 
Metaprogrammierung stattfinden und die #attr_xxx Methoden sollten 
grundsätzlich im gesamten Kernel Code Anwendung finden. Ebenso sollten Alias 
Methoden und solche die als _private_ gekennzeichnet sind direkt im Anschluss 
an die Methodendefinition geschrieben werden (letzteres mittels 
`private :sym`). Denke daran, dass die Methoden, die die Alpha Stage (alpha.rb) 
durchlaufen, lediglich ein :symbol Argument annehmen.

## Dokumentation
  
  * Nutze für die Dokumentation des Ruby Codes RDoc

  * Für C++ Code verwende Doxygen.

  * Für die im /doc Verzeichnis vorliegende Dokumentation verwende Markdown.
    Siehe [Markdown syntax](http://daringfireball.net/projects/markdown/syntax)
    Stelle die Textbreite auf 78 Zeichen und nutze harte Zeilenumbrüche.
