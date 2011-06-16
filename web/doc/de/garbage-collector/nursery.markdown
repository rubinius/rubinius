---
layout: doc_de
title: Krippe
previous: Freispeichersammlung (GC)
previous_url: garbage-collector
next: Junge Generation
next_url: garbage-collector/young-generation
review: true
---
Die erste Generation in der generationellen Freispeichersammlung von Rubinius
ist die Krippe. Der Lebenszyklus eines Objekts beginnt in der Krippe, wo es auch
erstellt (bereitgestellt) wird.

Objekte leben bis zur nächsten Sammlung in der Krippe. Wenn ein Objekt nach der
Sammlung immernoch lebendig ist, wir es in die Junge Generation verlagert. Eine
Sammlung sorgt selbstständig dafür, dass Objekte aus der Krippe verlagert
werden.
