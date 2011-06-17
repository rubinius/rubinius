---
layout: doc_de
title: Alte Generation
previous: Mittlere Generation
previous_url: garbage-collector/young-generation
next: Große Objekte
next_url: garbage-collector/large-objects
review: true
---

Alte Objekte sind Objekte, die aus der Mittleren Generation verlagert wurden,
nachdem sie x Sammlungen überstanden haben.

Autotune ist der Mechanismus, der benutzt wird, um die Anzahl der GC-Zyklen, die
für eine Sammlung der Alten Generation benötigt werden, dynamisch anzupassen.
Dieser kann ausgeschaltet werden oder es kann eine konstante Anzahl der nötigen
GC-Zyklen via `gc.lifetime` angegeben werden. `gc.lifetime` ist die
Ausgangsanzahl der Sammlungen, die für eine Sammlung der Alten Generation nötig
sind.

Weitere Informationen über die vorhandenen Konfigurationsvariablen sind in der
[Konfigurationsdatei der
VM](https://github.com/rubinius/rubinius/blob/master/vm/configuration.hpp)
einzusehen.
