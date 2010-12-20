---
layout: doc_de
title: Anleitung - Ein Ticket erstellen
previous: Anleitungen
previous_url: how-to
next: Einen Ruby Spec schreiben
next_url: how-to/write-a-ruby-spec
---

Den Rubinius Issuetracker findest du hier: <http://github.com/evanphx/rubinius/issues>.

Damit ein Ticket nützlich ist, sollte er prägnant, fokussiert und
umsetzbar sein. Falls er diese Kriterien nicht erfüllt, wird er
vermutlich nur für Unordnung sorgen. Deshalb sollten Tickets in
mindestens eine der folgenden Kategorien fallen:


  1. Eine präzise Kommandozeilenhistorie, welche aufzeigt, wie man das
     Programm installiert und aufruft sowie den Backtrace des
     aufgetretenen Fehlers widergibt.
  2. Ein kurzes Stück Beispielcode, welcher das Problem illustriert
     sowie ein Kommandozeilenbefehl um den Code auszuführen.
  3. Ein Patch **inklusive Specs, falls es noch keine äquivalenten
     gibt** sowie ein Auszug der darstellt, wie die Specs sich vor und
     nach Anwendung des Patches verhalten.

Falls dein Ticket nicht in eine der oben genannten Kategorien passt,
heißt das nicht, dass es ungültig ist. Das Problem sollte lediglich
nicht als Ticket aufgenommen werden.

  1. Falls es sich um ein Feature handelt, solltest du in Betrachtung
     ziehen, es auf der Mailingliste zur Diskussion zu stellen. Du
     kannst natürlich außerdem auch versuchen es selbst zu
     implementieren und zu zeigen, warum und wie dein Feature nützlich
     ist.

  2. Falls es sich um eine Bibliothek oder Gem handelt, welche nicht
     korrekt funktioniert nimm dir etwas Zeit und versuche den Fehler
     zu reproduzieren und beschreibe daraufhin die Reproduktion in
     einem neuen Ticket.


## Genereller Ablauf zur Erstellung eines Tickets

  1. Doppelte Überprüfung.

     1. Baue Rubinius komplett neu ('rake clean; rake') nachdem du
        'git pull' ausgeführt hast oder einen frischen clone des
        Repositories gemacht hast.
     2. Lies [Problemlösungen](/doc/en/getting-started/troubleshooting)
        um zu sehen ob eine der dortigen Anleitungen das Problem
        beheben kann.
     3. Lies [Specs](/doc/en/specs/).

  2. Gib deinem Ticket einen spezifischen, vorzugsweise kurzen Titel

  3. Versehe dein Ticket mit angemessenen Tags.

  4. Versehe dein Ticket mit hinreichend Details bzgl. dem Problem.

     *  Der Kommandozeilenbefehl um das Programm auszuführen.
     *  Der Backtrace oder das Ergebnis versus dem erwarteten Ergebnis.
     *  Informationen über deinen Rechner und System. `uname -a` ist
        normalerweise gut (falls es irgengwelche "unknown" Felder
        gibt, erkläre/beschreibe diese bitte.)


## Zusätzliche Instruktionen für Tickets mit angehängten Patches

  *  Können lediglich eine Menge an Specs sein.
  *  Patches müssen zusätzlich mit Specs versehen sein, es sei denn die
     Speccs existieren bereits.
  *  Relevante Teile der Spec Ausgabe und der exakte 'bin/mspec' Aufruf
     von dem existierenden oder hinzugefügten Spec *vor der
     Veränderung*
  *  Die Spec Ausgabe und der exakte 'bin/mspec' Aufruf, bei dem ein
     erfolgreicher Durchlauf *nach der Veränderung* zu sehen ist

  *  Zusätzliche Beschreibungen / Kommentare deines Patches und wie er
     das Problem löst. Insbesondere in Fällen, bei denen neue
     Funktionalität hinzugefügt wurde sollte darauf verwiesen werden,
     ob der Inhalt des Tickets bereits in #rubinius (IRC) oder
     ruby-dev (Mailingliste) diskutiert wurde.

Abgesehen von Fällen, bei denen es aus irgendwelchen Gründen unmöglich
ist, sollte 'git-format-patch' benutzt werden um den Patch zu
erstellen. Das macht das Einbringen des Patches wesentlich einfacher
und es erhält die korrekte Zuordnung. Ansonsten sollte ein 'unified
diff' verwendet werden.


## Beispiel für das Einreichen eines Patches

Angenommen, der folgende Spec existiert und schlägt fehl:

    describe "Kernel.format" do
      it "is accessible as a module function" do
        Kernel.format("%s", "hello").should == "hello"
      end
    end

1. Ticket Titel:

   "[PATCH] No method 'format' on Kernel (Module)"

2. Tags:

   "patch core spec"

3. Ticket Nachricht:

   The method 'format' is not available as a module function of Kernel.

       $ bin/mspec spec/ruby/core/kernel/format_spec.rb
       Started
       .E

       1)
       Kernel.format is accessible as a module function ERROR
       No method 'format' on Kernel (Module):

   The method 'format' already exists but has not been set as a module
   function.  This patch does so.

   After the patch is applied:

       $ bin/mspec spec/ruby/core/kernel/format_spec.rb
       Started
       ..

       Finished in 0.016031 seconds

       2 examples, 2 expectations, 0 failures, 0 errors

4. Anhang:

Schließlich, packe deinen Patch in ein gist und füge den Link dazu in
dein Ticket ein. Unten siehst du den Patch aus dem obigen Beispiel aus
Vollständigkeitsgründen:

    From c61cecce6442347ebbdf1ded7a5c0832c97582c1 Mon Sep 17 00:00:00 2001
    From: Brian Ford <bford@engineyard.com>
    Date: Sat, 19 Jan 2008 17:48:19 -0800
    Subject: [PATCH] Set Kernel#format as a module function.


    diff --git a/kernel/core/kernel.rb b/kernel/core/kernel.rb
    index 2d2e508..f2a382e 100644
    --- a/kernel/core/kernel.rb
    +++ b/kernel/core/kernel.rb
    @@ -150,6 +150,7 @@ module Kernel
       end
       alias_method :format, :sprintf
       module_function :sprintf
    +  module_function :format
       module_function :abort

       def puts(*a)

