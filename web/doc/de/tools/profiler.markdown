---
layout: doc_de
title: Profiler
previous: Debugger
previous_url: tools/debugger
next: Memory Analysis
next_url: tools/memory-analysis
---

Rubinius hat einen eingebauten Profiler, der eine genaue Zeitmessung für alle
durchlaufenen Methoden bietet. Der Profiler ist auf VM-Ebene implementiert und
bietet die erbrachten Daten für Ruby zur weiteren Evaluation.


## VM-Profiler

Zur Erstellung und Erhaltung des Profilers spielen folgende Teilnehmer eine
Rolle: VM, SharedState, ProfilerCollection und Profiler. Die VM Klasse ist eine
Thread-lokale Datenstruktur. Das heißt, jede VM-Instanz bekommt eine eigene
Profiler-Instanz zugewiesen. Die SharedState-Instanz hat eine
ProfilerCollection-Instanz, die den VMs zugeordnet ist und schließlich die
zusammenführten Ergebnisse aller Profiler generiert.

Der Profiler lebt und stirbt in seiner eigenen Umgebung. Der Profiler wird bei
seiner Erstellung an eine VM-Instanz weitergereicht, damit er zur
Datenerstellung Zugriff auf die VM hat. Das STATE Argument könnte an alle
Profiler-Methoden weitergegeben werden, muss aber nicht, weil es ausreicht dies
gleich bei der Erstellung des Profilers zu machen. Der Profiler verändert niemals
die VM-Instanz. Diese Trennung von Aufgaben muss auf jeden Fall erhalten bleiben.

Die VM-Instanz generiert die Profiler-Instanz nur bei Bedarf (lazy). Die VM
registriert den Profiler dann auch mit dem SharedState.

Der SharedState unterhält die ProfilerCollection-Instanz und reicht Aufrufe zur
Registrierung oder Löschung von Profilern weiter.

Die ProfilerCollection-Instanz gibt den Befehl, dass die VM-Instanz, die einem
Profiler zugeordnet ist, den Profiler auch entfert, wenn er gelöscht wird.


## Ruby-Profiler

In der Ruby-Welt sagt die Rubinius::Profiler::Instrumenter-Instanz nichts über
die Multi-Thread Realität in der VM aus. Die individuellen
C++-Profiler-Instanzen sind für Ruby unsichtbar. Wenn in Ruby eine
Profiler-Instanz erstellt wird, dann kann diese Instanz lediglich den VM-Profiler
starten und beenden. Wenn der Profiler gestoppt wird, werden die gesammelten
Daten in einem LookupTable zurückgegeben. Der Ruby Profiler-Code kann dann mit
der #show-Methode angezeigt werden.

Rubinius bietet eine kompatible Standardbibliothek profile.rb und profiler.rb.
Weitere Informationen finden sich in der MRI Dokumentation.

Betrachtet man lib/profiler.rb, so kann man die grundlegenden Schritte zur
Nutzung des Profilers in Ruby erkennen:

    # Eine Profiler-Instanz erstellen
    profiler = Rubinius::Profiler::Instrumenter.new

    # Den Profiler starten
    profiler.start

    # Den Profiler stoppen
    profiler.stop

    # Die Profiler-Daten abfragen
    data = profiler.info

    # Oder die Profiler-Daten ausdrucken
    profiler.show  # takes on IO object, defaults to STDOUT

Ebenso kann man eine Methode verwenden, die den Profiler mit einem Block
verwendet.

    # Eine Profiler-Instanz erstellen
    profiler = Rubinius::Profiler::Instrumenter.new

    # Ein Profil erstellen
    profiler.profile do
      # einige Aufgaben
    end

Die #profile-Methode startet den Profiler, macht ein yield, hält den Profiler an
und druckt die Profiler-Daten aus. Wird 'false' als Argument an #profile
weitergegeben, dann druckt es die Daten nicht aus. Nichtsdestotrotz werden die
Profiler-Daten als Rückgabewert von #profile zurückgegeben.


Wie man die Daten des Flat-Profiler liest
-----------------------------------------

Der Flat-Profiler gibt Ausgabe-Daten in den folgenden Spalten zurück:

### % time

Angabe der Zeitspanne in Prozent, die in dieser Methode verbracht wurde im
Verhältnis zur Gesamtzeit.


### cumulative seconds

Angabe der kumulierten Zeit, die in dieser Methode, sowie aller weiteren aus ihr
heraus rekursiv aufgerufenen Methoden verbracht wurde. Diese Methode kann auch
als die Wurzel im Aufrufbaum gesehen werden. Die Summe aller von ihr aus
aufgerufenen Methoden ist die kumlierte Sekundenzahl für diese Methode.


### self seconds

Die in dieser Methode verbrachte Zeit in Sekunden, ungeachtet der Aufrufe anderer
Methoden aus ihr selbst heraus.


### calls

Die Anzahl der Aufrufe, die an diese Methode gingen.


### self ms/call

Die Angabe der self seconds in Millisekunden geteilt durch die Gesamtzahl aller
Aufrufe.


### total ms/call

cumulative seconds als Millisekunden geteilt durch die Gesamtzahl aller Aufrufe.


### Beispiel einer Flat-Ausgabe

Das folgende Skript ist die Grundlage für die beiden unten angegebenen
Profiler-Beispiele:

    class F
      def foo(a)
        1 + a
      end

      def bar(a, b)
        foo(a) ** b
      end

      def work(a, b, n, m)
        n.times { |i| i + bar(a, b) }
        m.times { |i| foo(i) }
      end
    end

    p = Rubinius::Profiler::Instrumenter.new :sort => :self_seconds
    p.profile {
      F.new.work 2, 3, 10, 5
    }


Führt man das Skript mit 'bin/rbx script.rb' aus, sollte folgende Tabelle
ausgegeben werden:


      %   cumulative   self                self     total
     time   seconds   seconds      calls  ms/call  ms/call  name
    ------------------------------------------------------------
      9.64     0.00      0.00         10     0.00     0.00  Fixnum#**
     23.33     0.00      0.00          1     0.01     0.08  #toplevel
     12.64     0.00      0.00         10     0.00     0.00  F#bar
     15.38     0.00      0.00         15     0.00     0.00  F#work {}
     17.74     0.00      0.00          2     0.00     0.03  Integer#times
     19.29     0.00      0.00          1     0.01     0.06  F#work
      1.12     0.00      0.00          1     0.00     0.00  Class#new
      0.68     0.00      0.00         15     0.00     0.00  F#foo
      0.13     0.00      0.00          1     0.00     0.00  Class#allocate
      0.06     0.00      0.00          1     0.00     0.00  Object#initialize

    10 methods called a total of 57 times


Wie man die Graphen-Ausgabe ausliest
------------------------------------

Die Graphen-Ausgabe stellt man über eine Konfigurations-Option ein:

    -Xprofiler.graph

Mit dem vorhin genannten Skript wird der unten angegebene Graph ausgegeben.
Jeder "Eintrag" im Graphen hat drei Abschnitte: 1) Die Methode für den Eintrag,
genannt Primärzeile; 2) die Aufrufer der Primärmethode; und 3) die Methoden,
die die Primärmethode selber aufrief. Die Felder haben unterschiedliche
Bedeutungen in Bezug auf den Ort ihres Eintrags.

Für die Primärzeile gelten die folgenden Felder:


### index

Ein Index, der jeder Methode im Graphen zugeordnet ist, um cross-Referenzen
zwischen den Einträgen zu ermöglichen.


### % time

Die Zeitspanne, die in dieser Methode verbracht wurde als Prozentangabe der
Gesamtzeit, die für alle Methoden gebraucht wurde. Diese Angabe entspricht der
Flat-Ausgabe.


### self

Die in dieser Methode verbrachte Zeit in Sekunden, ungeachtet der Aufrufe anderer
Methoden aus ihr selbst heraus.


### children

Die Gesamtzeit, die in allen von die Methode aufgerufenen Methoden verbracht
wurde.


### called

Die Anzahl der Aufrufe, die an diese Methode gingen.


### name

Der Name der Methode gefolgt von der Indexnummer.

Die beiden Zeilen über der Primärzeile sind die Methoden, die die Primärmethode
aufrufen. Die Felder der Aufrufer haben folgende Bedeutung:


### self

Die Zeitspanne, die in dieser Methode verbracht wurde als Prozentangabe der
Gesamtzeit, die für alle Methoden gebraucht wurde. Diese Angabe entspricht der
Flat-Ausgabe.


### children

Die Zeit, die die Methode für Aufrufe an die Primärmethode verbrauchte.


### called

Das called-Feld hat zwei Teile, getrennt durch einen Schrägstrich. Links steht
die Anzahl der Aufrufe, die diese Methode an die Primärmethode gemacht hat und
rechts steht die Gesamtzahl der Aufrufe, die diese Methode gemacht hat.
In anderen Worten, die beiden Zahlen zusammen zeigen das Verhältnis zwischen
den Aufrufen an die Primärmethode zu allen anderen Methoden.


### name

Der Name der aufrufenden Methode gefolgt vom Index. Ist der Index [0], wird die
Methode im Grahen nicht dargestellt.


Die Zeilen unterhalb der Primärzeile listet Methoden auf, die die Primärmethode
aufruft.

Die Felder der aufgerufenen Methoden sind wie folgt:


### self

Die Zeitspanne, die in dieser Methode verbracht wurde als Prozentangabe der
Gesamtzeit, die für alle Methoden gebraucht wurde. Diese Angabe entspricht der
Flat-Ausgabe.

### children

Die Angabe ist ein Schätzwert der Zeit, die die Aufgerufenen Methoden brauchten,
als die Methode von der Primärmethode aufgerufen wurde. Die Schätzung basiert
auf dem Verhältnis zwischen der Zeit, die diese Methode brauchte, als sie von
der Primärmethode aufgerufen wurde zu der Zeit, die sie in sich selbst
verbrauchte.


### called

Das called-Feld hat zwei Teile, getrennt durch einen Schrägstrich. Links steht
die Anzahl der Aufrufe, die diese Methode von der Primärmethode bekam und
rechts steht die Gesamtzahl der Aufrufe, die an diese Methode gemacht wurden.


### name

Der Name der aufgerufenen Methode gefolgt von ihrem Index [N]. Ist kein Index
vorhanden, dann gibt es für diese Methode auch keinen Primäreintrag im Graphen.
Um auch solche Methoden anzeigen zu lassen, wird die '-Xprofiler.full_report'
Option verwendet, die den vollständigen Graphen generiert.


    index  % time     self  children         called       name
    ----------------------------------------------------------
                      0.00      0.00         10/20             F#bar [3]
    [1]       9.6     0.00      0.00         10           Fixnum#** [1]
    -------------------------------------------------------
    [2]      23.2     0.00      0.00          1           #toplevel [2]
                      0.00      0.00          1/1              Class#new [7]
                      0.00      0.00          1/1              F#work [6]
    -------------------------------------------------------
                      0.00      0.00         10/15             F#work {} [4]
    [3]      12.7     0.00      0.00         10           F#bar [3]
                      0.00      0.00         10/15             F#foo [8]
                      0.00      0.00         10/10             Fixnum#** [1]
    -------------------------------------------------------
                      0.00      0.00         15/15             Integer#times [5]
    [4]      15.4     0.00      0.00         15           F#work {} [4]
                      0.00      0.00         10/10             F#bar [3]
                      0.00      0.00          5/15             F#foo [8]
    -------------------------------------------------------
                      0.00      0.00          2/2              F#work [6]
    [5]      17.8     0.00      0.00          2           Integer#times [5]
                      0.00      0.00         15/15             F#work {} [4]
    -------------------------------------------------------
                      0.00      0.00          1/2              #toplevel [2]
    [6]      19.3     0.00      0.00          1           F#work [6]
                      0.00      0.00          2/2              Integer#times [5]
    -------------------------------------------------------
                      0.00      0.00          1/2              #toplevel [2]
    [7]       1.1     0.00      0.00          1           Class#new [7]
                      0.00      0.00          1/1              Object#initialize [10]
                      0.00      0.00          1/1              Class#allocate [9]
    -------------------------------------------------------
                      0.00      0.00         10/20             F#bar [3]
                      0.00      0.00          5/15             F#work {} [4]
    [8]       0.7     0.00      0.00         15           F#foo [8]
    -------------------------------------------------------
                      0.00      0.00          1/2              Class#new [7]
    [9]       0.1     0.00      0.00          1           Class#allocate [9]
    -------------------------------------------------------
                      0.00      0.00          1/2              Class#new [7]
    [10]      0.1     0.00      0.00          1           Object#initialize [10]
    -------------------------------------------------------

    10 methods called a total of 57 times

