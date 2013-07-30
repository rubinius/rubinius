---
layout: doc_de
title: Anleitung - Einen Blogeintrag schreiben
previous: Benchmarks schreiben
previous_url: how-to/write-benchmarks
next: Dokumentation schreiben
next_url: how-to/write-documentation
---

Der Rubinius Blog benutzt Jekyll und ist in die Webseite sowie die
Dokumentationsseiten integriert. Wir unterstützen und schätzen Gasteinträge 
über Erfahrungen, die beim Nutzen und Entwickeln von Rubinius gemacht wurden.

Für den Einstieg sollte sichergestellt sein, dass die `kramdown` and
`jekyll` Gems installiert sind.

    rbx gem install jekyll kramdown

Das bevorzugte Format für Blogeinträge ist Markdown. Falls allerding spezielle
Formatierungsoptionen benötigt werden, kann der Eintrag auch direkt in HTML 
verfasst werden.

1. Erstelle eine Datei in `web/_posts/` mit dem Format 
   `YYYY-MM-DD-perma-link.markdown` als Dateinamen
1. Schreibe den Eintrag
1. Im `web/` Verzeichnis, führe `rbx -S jekyll build` aus
1. Erstelle einen Commiteintrag mit allen Veränderungen im `web/` Verzeichnis.
1. Reiche einen Patch ein oder falls du Commitrechte besitzt push den
   Commiteintrag in den master Zweig.
1. Sag uns bescheid, wenn es einen neuen Blogeintrag gibt. Eventuell haben wir
   ein paar Rückmeldungen bzgl. deines Eintrags bevor er veröffentlicht wird.
