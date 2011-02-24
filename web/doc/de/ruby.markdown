---
layout: doc_de
title: Ruby
previous: Stilvorlage
previous_url: contributing/style-guide
next: Scripte
next_url: ruby/scripts
review: true
---

Um nachvollziehen zu können, wie Rubinius Ruby implementiert, sollte man 
versuchen einige Konzepte von Rubys Funktionsweise zu verstehen. Dieses Kapitel
soll dazu beitragen einige Merkmale von Rubinius an Hand von Beispielen aus
der Ruby Welt vorzustellen, die sicherlich schon vertraut sind. Die Rubinius
Dokumentation geht grundsätzlich davon aus, dass du dich mit Ruby auskennst
oder zumindest vertraut bist mit den Konzepten einer Virtuellen Maschine und
Compilern.

Zentral für alle Einheiten dieses Kapitels ist die Idee des _scope_, des
Gültigkeitsbereichs. Im Ruby Syntax ist der Gültigkeitsbereich normalerweise
ableitbar. Oder, anders ausgedrückt, es gibt keine Syntaxelemente, deren 
Hauptaufgabe es ist, den Gültigkeitsbereich festzulegen oder zu beschränken.
Aus diesem Grund kann es manchmal etwas verwirrend sein, über den
Gültigkeitsbereich zu reden. Ein Beispiel dazu ist eine übliche Definition
einer Methode:

    a = 5

    def fleissig(a)
      puts a * 2
    end

Hier haben wir die `#fleissig` Methode, die uns einen Namen gibt, unter dem wir
die Berechnung `puts a * 2` erreichen können. Die Methode definiert aber ebenso
einen geschlossenen Gültigkeitsbereich für die Variable `a`. Er ist 
geschlossen, weil das `a = 5` über der Methode keinen Bezug zu der Variable 
`a` in `#fleissig` hat.

Es wird oft gesagt, dass in Ruby alles ein Objekt sei. Das ist nicht ganz
zutreffend. _Fast_ alles ist ein Objekt. Einige Dinge die absolut 
notwendig sind, um Ruby Code laufen zu lassen, sind nicht notwendigerweise
Objekte, an die du herankommst. Deswegen hängt es in der "ausführenden 
Umgebung" stark von der Implementation ab, welche Dinge tatsächlich Objekte
sind. Der Gültigkeitsbereich ist eines davon.

Im Wesentlichen ist der Gültigkeitsbereich eine Idee im Kontext von Antworten
auf Fragen wie: Was ist hier der Wert von `self`? Welche lokalen Variablen
gibt es hier? Welchen Wert wird die konstante `APPLE` an dieser Stelle im 
Programmcode haben?

Jedes der folgenden Elemente aus Ruby wird unter dem Gesichtspunkt der 
Implementierung in Rubinius betrachtet und geschaut, wie sich der 
Gültigkeitsbereich in den jeweiligen Situationen äußert.

1. [Scripte](/doc/de/ruby/scripts/)
1. [Methoden](/doc/de/ruby/methods/)
1. [Konstanten](/doc/de/ruby/constants/)
1. [Klassen & Module](/doc/de/ruby/classes-and-modules/)
1. [Blöcke & Procs](/doc/de/ruby/blocks-and-procs/)
1. [Lokale Variablen](/doc/de/ruby/local-variables/)
1. [Instanzvariablen](/doc/de/ruby/instance-variables/)
1. [Klassenvariablen](/doc/de/ruby/class-variables/)
1. [Globale Variablen](/doc/de/ruby/global-variables/)
