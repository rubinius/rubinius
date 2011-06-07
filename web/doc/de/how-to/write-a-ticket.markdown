---
layout: doc_de
title: Anleitung - Ein Ticket erstellen
previous: Anleitungen
previous_url: how-to
next: Einen Ruby Spec schreiben
next_url: how-to/write-a-ruby-spec
---

Den Rubinius Issuetracker findest du hier: 
<http://github.com/rubinius/rubinius/issues>.

Damit ein Ticket nützlich ist, sollte es prägnant, fokussiert und umsetzbar 
sein. Falls es diese Kriterien nicht erfüllt, wird es vermutlich nur den 
Issuetracker zumüllen. Deshalb sollten Tickets mindestens einer der folgenden 
Kategorien zugeordnet werden können:

  1. Eine vollständige Kommandozeilenhistorie liegt vor, die aufzeigt, wie das 
     Programm installiert und aufgerufen wurde sowie den Backtrace des
     aufgetretenen Fehlers wiedergibt.
  2. Ein kurzes Stück Beispielcode, der das Problem illustriert sowie einein 
     Kommandozeilenbefehl, um den Code auszuführen.
  3. Ein Patch **inklusive Specs, falls es noch keine äquivalenten
     gibt** sowie ein Auszug der darstellt, wie die Specs sich vor und nach 
     Anwendung des Patches verhalten.

Falls die Angelegenheit nicht in eine der oben genannten Kategorien passt, 
heißt das nicht, dass sie unwichtig ist. Für Ticket ist es lediglich 
unzureichend.

  1. Falls es sich um ein Feature handelt, solltest du in Betracht ziehen, es 
     auf der Mailingliste zur Diskussion zu stellen. Natürlich kannst du 
     außerdem auch versuchen es selbst zu implementieren und zu zeigen, warum 
     und wie dein Feature nützlich sein kann.

  2. Falls es sich um eine Bibliothek oder Gem handelt, das nicht korrekt 
     funktioniert nimm dir etwas Zeit und versuche den Fehler zu reproduzieren
     und beschreibe daraufhin die Reproduktion in einem neuen Ticket.


## Genereller Ablauf zur Erstellung eines Tickets

  1. Doppelte Überprüfung.

     1. Kompiliere Rubinius komplett neu ('rake clean; rake') nachdem du 
        'git pull' ausgeführt hast oder einen frischen Klon des Repositories 
        erstellt hast.
     2. Lies [Problemlösungen](/doc/de/getting-started/troubleshooting),
        um zu sehen, ob eine der dortigen Anleitungen das Problem beheben kann.
     3. Lies [Specs](/doc/de/specs/).

  2. Gib deinem Ticket einen spezifischen, vorzugsweise kurzen Titel

  3. Beschreibe dein Ticket mit angemessenen Tags.

  4. Belege dein Ticket mit hinreichend Details bzgl. des Problems.

     *  Der Kommandozeilenbefehl, um das Programm auszuführen.
     *  Der Backtrace oder das Ergebnis entgegen dem erwarteten Ergebnis.
     *  Informationen über deinen Rechner und System. `uname -a` ist
        normalerweise gut (falls es irgengwelche "unknown" Felder
        gibt, erkläre/beschreibe diese bitte.)


## Zusätzliche Instruktionen für Tickets mit angehängten Patches

  *  Einfach nur ein paar Specs.
  *  Patches müssen zusätzlich mit Specs versehen sein, es sei denn die Specs 
     existieren bereits.
  *  Relevante Teile der Spec Ausgabe und der exakte 'bin/mspec' Aufruf von dem
     existierenden oder hinzugefügten Spec *vor* der Veränderung.
  *  Die Spec Ausgabe und der exakte 'bin/mspec' Aufruf, bei dem ein 
     erfolgreicher Durchlauf *nach* der Veränderung zu sehen ist.
  *  Zusätzliche Beschreibungen/Kommentare deines Patches und wie er das 
     Problem löst. Insbesondere dann, wenn neue Features hinzugefügt wurden, 
     sollte darauf verwiesen werden, ob der Inhalt des Tickets bereits in 
     #rubinius (IRC) oder ruby-dev (Mailingliste) diskutiert wurde.

Abgesehen von Fällen, bei denen es aus irgendwelchen Gründen unmöglich ist, 
sollte 'git-format-patch' benutzt werden, um den Patch zu erstellen. Das macht
das Einbringen des Patches wesentlich einfacher und er erhält die korrekte 
Zuordnung. Ansonsten sollte ein 'unified diff' verwendet werden.

## Beispiel für das Einreichen eines Patches

Angenommen, der folgende Spec existiert und schlägt fehl:

    describe "Kernel.format" do
      it "is accessible as a module function" do
        Kernel.format("%s", "Hallo").should == "Hallo"
      end
    end

1. Ticket Titel:

   "[PATCH] No method 'format' on Kernel (Module)"

2. Tags:

   "patch core spec"

3. Ticket Nachricht (<i>natürlich auf Englisch</i>)

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

Schließlich, packst du deinen Patch in ein Gist und fügst den Link dazu in
dein Ticket ein. Der Vollständigkeit halber siehst du unten den Patch aus dem 
obigen Beispiel:

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
