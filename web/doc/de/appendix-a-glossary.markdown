---
layout: doc_de
title: Anhang A - Glossar
previous: Anleitung - Übersetzen von Dokumentation
previous_url: how-to/translate-documentation
next: Anhang B - Literaturverzeichnis
next_url: appendix-b-reading-list
review: true
---

Eine Auflistung von Begriffsdefinitionen und Phrasen die in der
Programmiersprache Ruby und dieser Implementierung verwendet
werden. Siehe auch "The Ruby Programming Language" von Flanagan und
Matsumoto [O'Reilly 2008] und "Programming Ruby: The Pragmatic
Programmer's Guide" 2. oder 3. Auflage von Thomas et al [The Pragmatic Programmers
2005-2008]


* _method lookup oder method resolution_

  Es ist ganz einfach: Nimm das Objekt im 'class slot' des Objekts
  (welches nicht immer dem Rückgabewert von Object#class sein muss;
  wenn das Objekt eines hat, ist es die Metaklasse) und beginne die
  Suche.

  Der Suchvorgang geht die Superklassen-Kette aufwärts bis die
  Superklasse nil ist.

  In dem Fall beginne den Vorgang von vorn, nur suche diesmal nach
  method_missing. Schlägt die Suche nach method_missing fehl, wirf
  einen Fehler (Exception).

                                            +----------------+
                                            |      nil       |
                                            +----------------+
                                                    ^
                                                    | superclass
                                                    |
                                            +----------------+
                                            |     Object     |
                                            +----------------+
                                                    ^
                                                    | superclass
                                                    |
                                            +----------------+
                                            |     Module     |
                                            +----------------+
                                                    ^
                                                    | superclass
                                                    |
                                            +----------------+
                                            |     Class      |
                                            +----------------+
                                                    ^
                                                    | superclass
                                                    |
                                            +----------------+
                                            | SingletonClass |
                                            |    (Object)    |
                                            +----------------+
                                                    ^
                                                    | superclass
                                                    |
       +-------------+                      +----------------+
       |      F      |  ----------------->  | SingletonClass |
       +-------------+   singleton class    |      (F)       |
                                            +----------------+


      class Class
        def wanker
          puts 'you are'
        end
      end

      class F
        def self.bloke
          wanker
        end
      end

  1. Finde Methode 'wanker' -- Durchsuche Methodentabellen in:

      1. SingletonClass(F)
      1. SingletonClass(Object)
      1. Class

  Gefunden.


* _method_table (Methodentabelle)_

  Eine Datenstruktur die in jeder Klasse (und jedem Modul) vorhanden
  ist und die definierten Methoden der Klasse enthält.

  In Rubinius ist die Methodentabelle eine Instanz der Klasse
  LookupTable.


* _MatzRuby_

  Siehe MRI


* _MRI_

  Matz's Ruby Interpreter oder Matz's Ruby Implementation. Eine
  Abkürzung mit der die offizielle Implementierung von Ruby gemeint
  ist. Siehe <http://ruby-lang.org>.


* _private send (Privater Nachrichtenversand / Methodenaufruf)_

  Ein Methodenaufruf bei dem ein expliziter lexikalischer Empfänger
  (receiver) fehlt. Der Empfänger des Aufrufs ist +self+.
  Ein Beispiel:

      class A
      private
        def you_are_mine
        end
      end

      class B < A
        def sunshine
          you_are_mine
        end
      end

      class C
        def dear
          today = B.new
          today.you_are_mine
        end
      end

  Der Aufruf von +you_are_mine+ in der Methode +sunshine+ ist ein
  private Methodenaufruf (private send). Der Aufruf von
  +today.you_are_mine+ wird nicht gelingen, da private Methoden nicht
  einen expliziten Empfänger haben können. In diesem Fall ist +today+
  der explizite Empfänger.


* _metaclass (Metaklasse)_

  Auch +singleton+ class oder +eigenclass+ genannt. Jedes Objekt in
  Ruby kann eine singleton Klasse haben, wobei diese nur nach Bedarf
  erzeugt werden. Die Metaklasse enthält die Methoden- und
  Konstantentabellen welche jeweils nur zu einer bestimmten
  Objektinstanz gehören.
  Zum Beispiel existiert die unten definierte Methode +hello+ nur in
  der Metaklasse von +obj+.

      obj = Object.new
      def obj.hello
        puts 'hi'
      end

  Da alle Klassen in Ruby ebenfalls Objekte sind, können diese auch
  Metaklassen besitzen. "Klassenmethoden" sind somit lediglich
  Methoden, die in der Metaklasse einer Klasse definiert (d.h. in der
  Methodentabelle der Metaklasse eingetragen) sind. Die Methode +honk+
  existiert in der Metaklasse der Klasse +Car+.

      class Car
        def self.honk
        end
      end

  In Rubinius sind Metaklassen Instanzen der Klasse +SingletonClass+. Die
  Metaklasse eines Objektes kann durch den Aufruf der +metaclass+ Methode
  erhalten werden. Insgesamt wird die Anordnung der dabei beteiligten Konzepte
  als "Meta-Object Protocol" oder +MOP+ bezeichnet.


* _superclass (Superklasse)_

  Die Klasse von der eine bestimmte Klasse direkt erbt. Die Object
  Klasse ist die Superklasse aller Klassen die nicht explizit von
  einer anderen Klasse erben.

      class A
      end

      class B < A
      end

  Klasse A erbt von Object. Mit anderen Worten: A.superclass == Object.
  Klasse B erbt explizit von Klasse A. Also: B.superclass == A.
