---
layout: doc_de
title: Rubinius kompilieren
previous: Abhängigkeiten
previous_url: getting-started/requirements
next: Rubinius ausführen
next_url: getting-started/running-rubinius
---

Rubinius kann vom Quellverzeichnis aus kompiliert und ausgeführt
werden. Es muss nicht systemweit installiert werden, um laufen zu
können. Die folgende Anleitung beschreibt die Installation von
Rubinius sowie das Ausführen aus dem Quellverzeichnis.

Rubinius nutzt [LLVM](http://llvm.org/) für den eingebauten JIT (Just-in-time)
Compiler. Dabei hängt Rubinius von einer speziellen Version von LLVM
ab und setzt voraus, dass LLVM mit C++ RTTI (run-time type
information) Unterstützung gebaut wurde. Das `configure` Skript wird
automatisch diese Voraussetzungen überprüfen, wenn es nach einer
installierten Version von LLVM im System sucht. Falls du LLVM
installiert hast und die Voraussetzungen nicht erfüllt sind
bzw. Rubinius nicht korrekt mit der installierten Version von LLVM
kompiliert, kannst du `--skip-system` als Kommandozeilenflag beim
Aufruf des `configure` Skripts angeben. Dadurch wird automatisch die
korrekte Version von LLVM heruntergeladen und zusammen mit Rubinius
kompiliert.

### Beschaffen des Quellcodes

Der Rubinius Quellcode ist als Tarball und als Projekt auf Github
verfügbar. Der Tarball kann [hier heruntergeladen
werden](https://github.com/rubinius/rubinius/tarball/master).

Um Git zu nutzen:

  1. Wechsle in dein Entwicklungsverzeichnis
  2. `git clone git://github.com/rubinius/rubinius.git`


### Rubinius installieren

Wir empfehlen Rubinius in ein Verzeichnis zu installieren, bei dem man
weder `sudo` noch superuser Rechte braucht. Um Rubinius zu
installieren, befolge die folgenden Schritte:

  1. `./configure --prefix=/pfad/zum/installations/verzeichnis`
  2. `rake install`
  3. Folge den Anweisungen, um das _bin_ Verzeichnis von Rubinius deinem PATH hinzuzufügen

Falls du vorhast Rubinius zur Ausführung deiner Anwendung zu
nutzen, ist dies die bevorzugte Methode. Allerdings kannst du
Rubinius auch direkt aus dem Quellverzeichnis ausführen. Mehr dazu im
nächsten Abschnitt.

### Aus dem Quellverzeichnis ausführen

Für die Entwicklung an Rubinius selbst sollte diese Option gewählt
werden.

  1. `./configure`
  2. `rake`

Möchtest du Rubinius lediglich ausprobieren, folge den Anweisungen, damit
das _bin_ Verzeichnis deinem PATH hinzuzugefügt wird.

Falls du hingegen an Rubinius mitentwickeln willst, sollte das _bin_
Verzeichnis _nicht_ deinem PATH hinzugefügt werden, da Rubinius einen
eigenständigen Ruby Interpreter benötigt, um korrekt den bootstrap
Prozess während des Kompiliervorgangs durchführen zu können.
