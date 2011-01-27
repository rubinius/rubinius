---
layout: doc_de
title: Anleitung - Einen fehlgeschlagenen Spec reparieren
previous: Einen Ruby Spec schreiben
previous_url: how-to/write-a-ruby-spec
next: Einen Blogeintrag schreiben
next_url: how-to/write-a-blog-post
---

Die folgenden Abschnitte solltest du gelesen haben:

  *  [Getting Started]({{ page.lang_dir }}getting-started/)
  *  [Specs]({{ page.lang_dir }}specs/)

Danach folge diesen Schritten um einen fehlgeschlagenen Spec (Test) zu beheben:

  1.  Rufe `rake` auf um sicher zu stellen, dass alle CI Specs passieren.
  2.  Rufe `bin/mspec spec/some/spec_file.rb` auf um zu bestätigen, dass der entsprechende Spec fehlschlägt.
  3.  Bearbeite eine Quelldatei in Rubinius (vermutlich irgendwo innerhalb des kernel Verzeichnisses).
  4.  Rufe `rake build` auf um deine Veränderungen zu bauen.
  5.  Rufe `bin/mspec spec/some/spec_file.rb` auf um zu sehen ob deine Veränderungen den Spec zum laufen bringen.
  6.  Wiederhole die obigen Schritte, bis der Spec nicht mehr fehlschlägt.
  7.  Rufe `rake` auf um sicher zu stellen, dass es keine Regressionen gibt.
  8.  Wechsle in das root Verzeichnis von Rubinius, falls nocht nicht geschehen.
  9.  Rufe `git status, git add, git commit` etc. auf. Jegliche
      Veränderungen die in den Spec Dateien gemacht wurden (innerhalb des
      spec/ruby Verzeichnisses) müssen in einem unterschiedlichen Commit
      eingetragen werden als die Veränderungen, die am restlichen Rubinius
      Quellcode gemacht wurden.
  10. Rufe `git format-patch origin` auf, wodurch alle Commiteinträge die im aktuellen Zweig seit dem letzten Pull von origin gemacht wurden extrahiert werden.
      Alternativ, rufe `git format-patch -N' auf, wobei N die Zahl der Commiteinträge ist (1, 2 etc.) für die du Patches erstellen willst.
  11. Erstelle ein gist (auf http://gist.github.com) mit deinem Patch und verlinke es in einem neuen Ticket im Issuetracker auf Github: http://github.com/evanphx/rubinius/issues
      Es können mehrere Patches einem Ticket hinzugefügt werden.

Wenn dein Patch vom Rubinius Projekt akzeptiert wurde erhälst du ein
sog. Commit Bit, d.h. Schreibzugriff, auf das Rubinius Git
Repository. Gib Evan bescheid, wie dein Github Benutzername ist.
