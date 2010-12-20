---
layout: doc_de
title: Anleitung - Dokumentation schreiben
previous: Einen Blogeintrag schreiben
previous_url: how-to/write-a-blog-post
next: Dokumentation übersetzen
next_url: how-to/translate-documentation
---

Die Dokumentation für Rubinius ist integriert mit der Website und dem
Blog. Es nutzt Jekyll genauso wie die übrigen Komponenten.

Um zu starten, stelle sicher, dass du die `kramdown` und `jekyll` Gems
installiert hast.

    rbx gem install jekyll kramdown

Die Dokumentationsquellen befinden sich im `web/doc` Verzeichnis. Es
existieren dort Unterverzeichnisse für jede Sprache, für die es
bereits eine Übersetzung gibt (wie z.B. der deutschen Übersetzung
hier).

Jede Übersetzung besitzt ein Inhaltsverzeichnis
(z.B. `/web/doc/en/index.markdown`). Der Rest der Dokumentation
besteht aus einzelnen Dateien, welche YAML Attribute benutzen um
anzugeben, wie die Dokumente zueinander in Beziehung stehen. Im
Prinzip kann man die Dokumentation als eine doppelt verkettete Liste
von Dokumenten verstehen, bei der jedes Dokument auf ein Vorgänger-
sowie einen Nachfolgedokument verweist. Das Inhaltsverzeichnis
Dokument zeigt dabei die komplette Struktur aller übrigen Dokumente
an.

Die YAML Attribute in einem Dokument sehen in etwa wie folgt aus:

    ---
    layout: doc_en
    title: How-To - Write Documentation
    previous: Write a Blog Post
    previous_url: how-to/write-a-blog-post
    next: Translate Documentation
    next_url: how-to/translate-documentation
    ---

Das _layout_ gibt an, welches Jekyll Layout bei der Formatierung des
Dokuments genutzt werden soll. Das _layout_ sollte `doc_LANG`
entsprechen, wobei _LANG_ ein ISO-639-1 Code für die Sprache ist.

Das _title_ gibt den Dokumententitel an, welcher am oberen Rand der Seite dargestellt wird.

Die _previous_ und _previous\_url_ Attribute stellen den Titel sowie
Link zum vorherigen Dokument dar. Gleichermaßen entsprechen die _next_
und _next\_url_ Attribute den Titel und Link zum folgenden
Dokument. Dies wird benutzt um das Lesen und Durchstöbern der
Dokumentationsseiten zu vereinfachen und den Arbeitsaufwand beim
Neuordnen von Dokumentationsabschnitten zu minimieren.


### Vorhandene Dokumentation bearbeiten

Ein anfänglicher Entwurf für die Dokumentation wurde bereits
erstellt. Es gibt aber noch viele Themen, welche noch Dokumentation
bedürfen.

Um Dokumentation für ein bestehendes Thema hinzuzufügen oder
vorhandene Dokumentation zu verbessern, öffne die Datei für das
jeweilige Thema unter `web/doc/LANG` und füge dort neue Dokumentation
hinzu oder überarbeite diese.


### Neue Dokumentation hinzufügen

Um neue Dokumentation für bisher nicht dokumentierte Themen hinzuzufügen:

1. Erstelle eine neue Datei mit der .markdown Dateiendung unter `web/doc/LANG`.

1. Richte die YAML Attribute ein, sodass auf die neue Datei innerhalb
   der existierenden Dokumente verlinkt wird. Dies erfordert das
   Editieren der _previous_ and _next_ Attribute der existierenden
   Dateien um die neue Datei einzufügen sowie das Hinzufügen eines
   Eintrags in `index.markdown`.
1. Um deine Veränderungen zu sehen, während du daran arbeitest kannst
   du `rbx -S jekyll --server --auto` ausführen.
1. Editieren die neue Datei mit der Markdown Syntax.
1. Im `web/` Verzeichnis, führe `rbx -S jekyll` aus.
1. Committe alle Veränderungen im `web/` Verzeichnis.
