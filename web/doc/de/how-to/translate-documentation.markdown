---
layout: doc_de
title: Anleitung - Dokumentation übersetzen
previous: Dokumentation schreiben
previous_url: how-to/write-documentation
next: Anhang A - Glossar
next_url: appendix-a-glossary
---

Die zwei wichtigsten Hilfen beim Übersetzen:

1. Eine existierende Übersetzung aktualisieren oder verbessern
1. Eine komplett neue Übersetzung anlegen

Zunächst sollte 
[Anleitung - Schreibe Dokumentation](/doc/de/how-to/write-documentation/) 
gelesen werden.


### Eine existierende Übersetzung aktualisieren oder verbessern

Um eine existierende Übersetzung zu aktualisieren, öffne eine Themendatei unter
`web/doc/LANG` und bearbeite den vorhandenen Text oder füge neu übersetzten 
Text ein.


### Eine neue Übersetzung anlegen

Um eine neue Übersetzung für eine weitere Sprache anzulegen, gehe wie folgt 
vor:

1. Kopiere `web/doc/en` nach `web/doc/LANG`, wobei _LANG_ der
   [ISO-639-1](http://en.wikipedia.org/wiki/List_of_ISO_639-2_codes) Code ist,
   der beim Anlegen einer neuen Sprache maßgeblich ist.
1. Bearbeite die Links im Inhaltsverzeichnis so, dass diese auf die übersetzten
   Dateien verweisen. (Beachte, dass aus derzeit unbekannten Gründen das 
   `page.base_dir` Attribut nicht verfügbar ist, wenn diese Dateien von Jekyll
   generiert werden. Das Problem muss noch untersucht werden.)
1. Übersetzte (nach Möglichkeit) den englischen Text in die Zielsprache, damit
   die Übersetzung möglichst aktuell ist.
