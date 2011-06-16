---
layout: doc_de
title: Junge Generation
previous: Krippe
previous_url: garbage-collector/nursery
next: Alte Generation
next_url: garbage-collector/mature-generation
review: true
---
Objekte die seit mehr als eines GC-Zyklus lebendig sind, leben im Raum der
Jungen Generation. Ein Durchlauf der Freispeichersammlung sorgt selbstständig
dafür, dass Objecte in die Junge Generation verlagert werden.

Objekte leben für X Sammlungen im Raum der Jungen Generation. X wird, abhängig
davon, wie viele Objekte in die Alte Generation befördert werden, dynamisch
angepasst.

Wenn *zu viele* Objekte während eines GC-Zyklus befördert werden, steigt X.
Wenn *nicht genug* Objekte während eines GC-Zyklus befördert werden, sinkt X.

Dies wird getan, um die Beförderungsrate stabil zu halten. *Stabil* bedeutet
dabei eine nur geringe Schwankung in der Anzahl der Beförderungen.
