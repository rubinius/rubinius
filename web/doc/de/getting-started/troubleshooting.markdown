---
layout: doc_de
title: Problemlösungen
previous: Rubinius ausführen
previous_url: getting-started/running-rubinius
next: Mitmachen
next_url: contributing
---

Im Folgenden werden Fehler beschrieben, die während des Kompilierens
oder der Installation auftreten können sowie mögliche Ansätze zur Fehlerbehebung.

Sollte es vorkommen, dass ein Fehler auftritt, stelle zuerst sicher, dass eine
aktuelle, sauber ausgecheckte (clean checkout) Version von Rubinius
vorliegt. Dazu können folgende Befehle im Rubinius Verzeichnis ausgeführt werden:

    $ git co master
    $ git reset --hard
    $ git pull
    $ rake distclean
    $ rake


möglicher Fehler:

    ERROR: unable to find runtime directory

    Rubinius was configured to find the runtime directory at:

      /Users/brian/devel/rubinius/runtime

    but that directory does not exist.

    Set the environment variable RBX_RUNTIME to the location
    of the directory with the compiled Rubinius kernel files.

    You may have configured Rubinius for a different install
    directory but you have not run 'rake install' yet.

Lösung:

  Falls Rubinius via `--prefix` konfiguriert wurde, führe `rake install` aus.

  Falls Rubinius via `--prefix` konfiguriert wurde und das
  Installationsverzeichnis nach der Installation umbenannt wurde, muss
  Rubinius neu konfiguriert und installiert werden.

  Das gleiche gilt für das Quellverzeichnis, das nach dem Kompilieren umbenannt
  wurde. Rubinius muss neu konfiguriert und installiert werden.

  Generell sollte das Quell- oder Kompilierverzeichnis nicht nach dem Kompilieren
  umbenannt werden.

