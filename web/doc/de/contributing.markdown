---
layout: doc_de
title: Mitmachen
previous: Problemlösungen
previous_url: getting-started/troubleshooting
next: Kommunikation
next_url: contributing/communication
---

Das Rubinius Projekt freut sich über deine Beiträge. Es gibt viele Dinge, die
dieses Projekt unterstützen können. Am wichtigsten ist, dass es etwas ist, was
dich interessiert und was du gerne machst.

Falls Du Fragen zu Rubinius hast, besuche uns am besten auf dem IRC Kanal
#rubinius auf irc.freenode.net.

Im folgenden findest du ein paar Ideen, was man für Rubinius tun könnte.

## Teste Deinen Code

Anwendungen sind oft weniger sanft als specs und decken Fehler auf, die unsere
Tests auslassen. Lass dein Projekt unter Rubinius laufen und melde Fehler.
Siehe hier, [wie man ein Ticket schreibt](/doc/de/how-to/write-a-ticket).

## Hilfe

Soweit es uns möglich ist, werden wir helfen. Bitte sieh aber auch nach, ob Du
im Netz eventuell schon Antworten auf deine Fragen findest. Rubinius möchte
ein Projekt sein, in das man sich leicht einarbeiten kann, an dem man lernt
und natürlich auch beteiligt kann.

Wir schätzen und nehmen einfache Fehlermeldungen gerne an, geben aber Tickets
mit ausführlichen Fehlerberichten und Anweisungen zur Reproduktion des Fehlers
eine höhere Priorität. Noch besser sind Fehlermeldungen, die RubySpecs
beinhalten, die den Fehler genau eingrenzen und womöglich schon Patches
mitliefern, die den Fehler beheben.

## Schreibe Specs

  1. Starte `bin/mspec tag --list incomplete <dir>`, um zu zeigen, dass die
     Specs als unvollständig markiert sind. Diese Specs müssen dann häufig nur
     nachgeprüft werden oder es fehlen einer bestimmten Klasse noch Specs.
    
     Hinweis: Du kannst auch ein pseudo-Verzeichnis ':files' für \<dir\>
     angeben, das für alle auf Rubinius laufenden Specs die Tags anzeigt. Oder
     du gibst ein Unterverzeichnis des 'spec/' Verzeichnisses an, um nur die
     Tags für dieses Verzeichnis anzuzeigen.
    
  2. Finde nicht vorgegebenes Verhalten. Siehe auch,
     [wie man Ruby Specs schreibt](/doc/de/how-to/write-a-ruby-spec).



## Fehlerhafte Specs korrigieren
  1. Starte `bin/mspec tag --list fails <dir>` zum Anzeigen von erfolglosen
     Specs.
     
     Hinweis: Du kannst auch ein pseudo-Verzeichnis ':files' für \<dir\>
     angeben, das für alle auf Rubinius laufenden Specs die Tags anzeigt. Oder
     du gibst ein Unterverzeichnis des 'spec/' Verzeichnisses an, um nur die
     Tags für dieses Verzeichnis anzuzeigen.

  2. Wähle ein Spec, dass Du interessant findest und versuche einen Patch zu
     schreiben, dass ihn bestehen lässt.

## Dokumentation

Lerne wie Rubinius funktioniert und schreibe deine Erfahrungen auf, damit
andere ebenso davon profitieren und verstehen, wie Rubinius implementiert ist.


## Tickets sichten
  * Alte Tickets prüfen oder schließen
  * Minitests schreiben, die die Fehler reproduzieren. Anschließend prüfen,
    ob es schon Specs für diese Fälle gibt. Wenn nicht, evtl. welche schreiben.
    