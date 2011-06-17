---
layout: doc_de
title: Mittlere Generation
previous: Junge Generation
previous_url: garbage-collector/nursery
next: Alte Generation
next_url: garbage-collector/mature-generation
review: true
---

Objekte, die mehr als einen GC-Zyklus aktuell sind, werden Mitglied der
Mittleren Generation. Ein Durchlauf des GC sorgt selbstständig dafür, dass
Objekte in die Mittlere Generation verlagert werden.

Objekte verbleiben für x Sammlungen im Raum der Mittleren Generation. x wird,
abhängig davon, wie viele Objekte in die Alte Generation verlagert werden,
dynamisch angepasst.

Wenn *zu viele* Objekte während eines GC-Zyklus verlagert werden, steigt x.
Wenn *nicht genug* Objekte während eines GC-Zyklus verlagert werden, sinkt x.

Diese Regel besteht, damit die Verlagerungsrate konstant bleibt. Konstant
bedeutet in diesem Kontext eine geringe Schwankung in der Anzahl der
Verlagerungen pro Zyklus zu erlangen.

