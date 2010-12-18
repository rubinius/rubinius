---
layout: doc_de
title: Anleitung - Einen Ruby Spec schreiben
previous: Ein Ticket erstellen
previous_url: how-to/write-a-ticket
next: Einen fehlgeschlagenen Spec reparieren
next_url: how-to/fix-a-failing-spec
---

Du solltest folgende Artikel gelesen haben:

  *  [Erste Schritte](/doc/en/getting-started/)
  *  [Specs](/doc/en/specs/)

Anschließend folge diesen Schritten um einen Spec für eine Ruby Methode zu schreiben:

  1. Bearbeite eine Datei unter `spec/ruby/...`
  2. Führe `bin/mspec -tr spec/ruby/some/spec_file.rb` aus
  3. Wiederhole die obigen Schritte so lange, bis der Spec auf MatzRuby (MRI) passiert
  4. Committe deine Veränderungen
  7. Führe `git format-patch` aus
  8. Erstelle ein gist (auf http://gist.github.com) mit deinem Patch
     und verlinke diesen in einem Ticket auf dem Issuetracker:
     <http://github.com/evanphx/rubinius/issues>
