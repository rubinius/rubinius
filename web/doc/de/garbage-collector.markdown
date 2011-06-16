---
layout: doc_de
title: Freispeichersammlung (GC)
previous: JIT Compiler
previous_url: jit
next: Krippe
next_url: garbage-collector/nursery
review: true
---

## Einführung

Rubinius implementiert eine [generationelle Freispeichersammlung (GC)](http://de.wikipedia.org/wiki/Garbage_Collector#Generationell).
Die generationelle Freispeichersammlung von Rubinius regelt die dynamische
Bereitstellung und Freigabe im Freispeicher eines Rubinius-Prozesses.

## Definitionen

1. Wir sagen, ein Object ist *lebendig*, wenn es mindestens eine Struktur
   (Variable) im Interpreter gibt, welches dieses Object referenziert.
1. Einen Durchlauf der Freispeichersammlung nennt man *Sammlung*.
1. Wie oft die Freispeichersammlung ausgeführt wurde, nennt man die Anzahl der
   *GC-Zyklen* oder *GC-Sammlungen*.

## Objektlebenszyklus

1. [Krippe](/doc/de/garbage-collector/nursery/)
1. [Junge Generation](/doc/de/garbage-collector/young-generation/)
1. [Alte Generation](/doc/de/garbage-collector/mature-generation/)
1. [Große Objecte](/doc/de/garbage-collector/large-objects/)
