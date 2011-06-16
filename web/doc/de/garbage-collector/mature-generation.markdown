---
layout: doc_de
title: Alte Generation
previous: Young Generation
previous_url: garbage-collector/young-generation
next: Große Objecte
next_url: garbage-collector/large-objects
review: true
---
Alte Objekte sind Objekte, die aus der Jungen Generation befördert wurden,
nachdem sie die Beförderungsgrenze von X Sammlungen überlebt haben.

Autotune ist der Mechanismus, der benutzt wird, um die Anzahl der GC-Zyklen, die
für eine Sammlung der Alten Generation benötigt werden, dynamisch anzupassen.
Die kann ausgeschaltet werden oder es kann eine konstante Anzahl vie gc.lifetime
genutzt werden. gc.lifetime ist die Ausgangsanzahl der Sammlungen, die für eine
Sammlung der Alten Generation nötig sind.

Für mehr Informationen über die vorhandenen Konfigurationsvariablen wird auf die
[Konfigurationsquelldatei der VM](https://github.com/rubinius/rubinius/blob/master/vm/configuration.hpp)
verwiesen.

