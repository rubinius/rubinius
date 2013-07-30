---
layout: doc_de
title: Anleitung - Dokumentation schreiben
previous: Einen Blogeintrag schreiben
previous_url: how-to/write-a-blog-post
next: Dokumentation übersetzen
next_url: how-to/translate-documentation
---

Die Dokumentation für Rubinius ist integriert mit der Website und dem Blog. Sie
nutzt genau wie die übrigen Komponenten Jekyll.

Um zu starten, stelle sicher, dass du die `kramdown` und `jekyll` Gems 
installiert hast.

    rbx gem install jekyll kramdown

Die Dokumentationsquellen befinden sich im `web/doc` Verzeichnis. Es existieren
dort Unterverzeichnisse für alle Sprachen, die bereits in einer Übersetzung 
vorliegen (wie z.B. diese deutschen Übersetzung).

Jede Übersetzung besitzt ein Inhaltsverzeichnis
(z.B. `/web/doc/en/index.markdown`). Der Rest der Dokumentation besteht aus 
einzelnen Dateien, die YAML Attribute benutzen, um anzugeben, wie die Dokumente
zueinander in Beziehung stehen. Im Prinzip kann man die Dokumentation als eine
doppelt verkettete Liste von Dokumenten verstehen, bei der jedes Dokument auf 
ein Vorgänger- sowie einen Nachfolgedokument verweist. Das Inhaltsverzeichnis 
zeigt dabei die komplette Struktur aller übrigen Dokumente an.

Die YAML Attribute in einem Dokument sehen in etwa wie folgt aus:

    ---
    layout: doc_en
    title: How-To - Write Documentation
    previous: Write a Blog Post
    previous_url: how-to/write-a-blog-post
    next: Translate Documentation
    next_url: how-to/translate-documentation
    ---

Das _layout_ gibt an, welches Jekyll Layout bei der Formatierung des Dokuments
genutzt werden soll. Das _layout_ sollte `doc_LANG` entsprechen, wobei _LANG_ 
ein ISO-639-1 Code für die Sprache ist (z.B. de, en, pl, es etc.).

Das _title_ gibt den Dokumententitel an, der am oberen Rand der Seite 
dargestellt wird.

Die _previous_ und _previous\_url_ Attribute stellen den Titel sowie Link zum 
vorherigen Dokument dar. Gleichermaßen entsprechen die _next_ und _next\_url_ 
Attribute dem Titel und Link zum folgenden Dokument. Diese werden benutzt, um 
das Lesen und Durchstöbern der Dokumentation zu vereinfachen und den 
Arbeitsaufwand beim Neuordnen von Dokumentationsabschnitten zu minimieren.

### Vorhandene Dokumentation bearbeiten

Ein anfänglicher Entwurf für die Dokumentation wurde bereits erstellt. Es gibt
aber noch viele Themen, die einer vertiefenden Dokumentation bedürfen.

Um Dokumentation für ein bestehendes Thema hinzuzufügen oder vorhandene 
Dokumentation zu verbessern, öffne die Datei für das jeweilige Thema unter 
`web/doc/LANG` und füge dort neue Dokumentation hinzu oder überarbeite diese.

### Neue Dokumentation hinzufügen

Um neue Dokumentation für bisher nicht dokumentierte Themen hinzuzufügen:

1. Erstelle eine neue Datei mit der .markdown Dateiendung unter `web/doc/LANG`.
1. Richte die YAML Attribute ein, sodass auf die neue Datei innerhalb der 
   existierenden Dokumente verlinkt wird. Dies erfordert das Bearbeiten der 
   _previous_ and _next_ Attribute der bereits vorhandenen Dateien, um die neue
   Datei einzufügen. Ebenso muss ein Eintrag in der `index.markdown` gemacht 
   werden.
1. Damit du Veränderungen noch beim Bearbeiten überprüfen kannst, führe 
   folgenden Befehl aus und besuche die vom Server angegebene URL:
   `rbx -S jekyll serve --watch`
1. Bearbeite die neue Datei mit dem Markdown Syntax.
1. Im `web/` Verzeichnis, führe `rbx -S jekyll build` aus.
1. Mach ein Commit für alle Veränderungen im `web/` Verzeichnis.
