---
layout: doc_de
title: Große Objekte
previous: Alte Generation
previous_url: garbage-collector/mature-generation
next: Rubinius Systeme
next_url: systems
review: true
---
Große Objekte werden statt in der Krippe in der Neugeborenenwanne geboren.

Jedes Objekt, das die definierte bytezahl für große Objekte überschreitet, wird
direkt im Raum der großen Objekte angelegt. Dies wird getan, um das Kopieren zu
vermeiden, da das Kopieren von großen Objekte eine zeitaufwendige Operation ist.

Bemerkt sei, dass man die Anzahl der Bytes die eine *großes Objekt* ausmachen,
anpassen kann. Der Standard hierbei sind 2700 Bytes.

    rbx ... -Xgc.large_object=<Anzahl der Bytes> ...

Für mehr Informationen über die vorhandenen Konfigurationsvariablen wird auf die
[Konfigurationsquelldatei der VM](https://github.com/rubinius/rubinius/blob/master/vm/configuration.hpp)
verwiesen.

