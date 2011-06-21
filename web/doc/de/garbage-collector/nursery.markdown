---
layout: doc_de
title: Junge Generation
previous: Garbage Collector (GC)
previous_url: garbage-collector
next: Mittlere Generation
next_url: garbage-collector/young-generation
review: true
---

Die erste Generation in der generationellen Garbage Collection von Rubinius ist
die Junge Generation. Der Lebenszyklus eines Objekts beginnt in der Jungen
Generation, wo es auch erstellt (bereitgestellt) wird.

Objekte sind bis zur nächsten Sammlung Mitglied der Jungen Generation. Wenn ein
Objekt nach der Sammlung immer noch aktuell ist, wird es in die Mittlere
Generation verlagert. Eine Sammlung sorgt selbstständig dafür, dass Objekte
verlagert werden.

