---
layout: doc_de
title: Problemlösungen
previous: Rubinius ausführen
previous_url: getting-started/running-rubinius
next: Mitmachen
next_url: contributing
---

Im Folgenden werden Fehler beschrieben, welche während des Bauvorgangs
oder der Installation auftreten können sowie mögliche Fehlerbehebungsansätze.

Im Falle eines Fehler sollte zunächst sichergestellt sein, dass eine
aktuelle, sauber ausgecheckte (clean checkout) Version von Rubinius
vorliegt. Dazu können folgende Befehle ausgeführt werden:

    $ git co master
    $ git reset --hard
    $ git pull
    $ rake distclean
    $ rake


Fehler:

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
  Installationsverzeichnis nach der Installation umbenannt wurde muss
  Rubinius neu konfiguriert und installiert werden.

  Falls das Quellverzeichnis nach dem Bauvorgang umbenannt wurde muss
  Rubinius neu konfiguriert und installiert werden.

  Generell sollte das Quell- oder Bauverzeichnis nicht nach dem Bauvorgang umbenannt werden.