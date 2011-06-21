---
layout: doc_de
title: Garbage Collector (GC)
previous: JIT Compiler
previous_url: jit
next: Junge Generation
next_url: garbage-collector/nursery
review: true
---

## Einführung

Rubinius implementiert einen [Generationellen Garbage Collector
(GC)](http://de.wikipedia.org/wiki/Garbage_Collector#Generationell).  Der
Generationelle Garbage Collector von Rubinius regelt die dynamische
Bereitstellung und Freigabe im Speicher eines Rubinius-Prozesses.

## Definitionen

1. Ein Objekt ist *aktuell*, wenn es mindestens eine Struktur (Variable) im
   Interpreter gibt, die dieses Objekt referenziert.
1. Einen Durchlauf des GC nennt man *Sammlung*.
1. Die Anzahl der GC-Durchläufe nennt man *GC-Zyklen* oder *GC-Sammlungen*.

## Lebenszyklus eines Objekts

1. [Junge Generation](/doc/de/garbage-collector/nursery/)
1. [Mittlere Generation](/doc/de/garbage-collector/young-generation/)
1. [Alte Generation](/doc/de/garbage-collector/mature-generation/)
1. [Große Objekte](/doc/de/garbage-collector/large-objects/)

