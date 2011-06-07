---
layout: doc_de
title: Anleitung - Einen Ruby Spec schreiben
previous: Ein Ticket erstellen
previous_url: how-to/write-a-ticket
next: Einen fehlgeschlagenen Spec reparieren
next_url: how-to/fix-a-failing-spec
---

Du solltest folgende Artikel gelesen haben:

  *  [Erste Schritte](/doc/de/getting-started/)
  *  [Specs](/doc/de/specs/)

Anschließend folge diesen Schritten, um einen Spec für eine Ruby Methode zu 
schreiben:

  1. Bearbeite eine Datei unter `spec/ruby/...`
  2. Führe `bin/mspec -tr spec/ruby/some/spec_file.rb` aus
  3. Wiederhole die obigen Schritte so lange, bis der Spec auf MatzRuby (MRI) 
     besteht
  4. Sende ein Commit mit deinen Anderungen
  7. Führe `git format-patch` aus
  8. Erstelle ein Gist (auf http://gist.github.com) mit deinem Patch und 
     verlinke diesen in einem Ticket auf dem Issuetracker:
     <http://github.com/rubinius/rubinius/issues>
