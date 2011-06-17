---
layout: doc_de
title: Große Objekte
previous: Alte Generation
previous_url: garbage-collector/mature-generation
next: Rubinius Systeme
next_url: systems
review: true
---

Jedes Objekt, das eine definierte Bytezahl für große Objekte überschreitet, wird
direkt im Speicher für großen Objekte angelegt. Auf diese Weise lassen sich
zeit- und kostenaufwändige Operationen für Kopien vermeiden.

Bemerkt sei, dass man die Anzahl der Bytes die eine *großes Objekt* ausmachen,
anpassen kann. Der Standard hierbei sind 2700 Bytes.

    rbx … -Xgc.large_object=<Anzahl der Bytes> …

Weitere Informationen über die vorhandenen Konfigurationsvariablen sind in der
[Konfigurationsdatei der
VM](https://github.com/rubinius/rubinius/blob/master/vm/configuration.hpp)
einzusehen.

