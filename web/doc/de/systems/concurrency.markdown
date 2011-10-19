---
layout: doc_de
title: Concurrency
previous: FFI
previous_url: systems/ffi
next: IO
next_url: systems/io
review: true
---

Rubinius unterstützt die selben Konstrukte zur Nebenläufigkeit wie Ruby, Threads
und Fibers, dazu aber noch eine neue API: Actors. Actors stellen Nebenläufigkeit
ohne wechselseitigen Ausschluss (Mutex) oder Zugriffssperren (Locking) beim
gegenseitigen Austausch von Zuständen zwischen Threads zur Verfügung.

Actors führen nebenläufig aus, teilen sich aber keine Zustände. Anstelle dessen
schicken sie Mitteilungen an andere Actors. Im folgenden Beispiel werden zwei
Actors, ping und pong, mit Hilfe eines `Actor.spawn` erstellt. Beide schicken
sich Mitteilungen zu, so lange bis sie gemeinsam eine Variable auf 100 erhöht
haben:

    require 'actor'
    pong = nil
    ping = Actor.spawn do
      loop do
        count = Actor.receive
        break puts(count) if count > 1000
        pong << (count + 1)
      end
    end
    pong = Actor.spawn do
      loop do
        count = Actor.receive
        break puts(count) if count > 1000
        ping << (count + 1)
      end
    end
    ping << 1
    sleep 1

Actors empfangen Mitteilungen über `Actor.receive`. Das blockiert den Actor so
lange, bis eine Mitteilung eingeht. Unterschiedliche Arten von Mitteilungen
können in einem an `Actor.receive` angehängen Block mit einem Mitteilungs-Filter
verarbeitet werden:

    Actor.receive do |filter|
      filter.when(Ready) do |who|
        # SNIP
      end
      filter.when(Work) do |work|
        ready_workers.pop << work
      end
      filter.when(Actor::DeadActorError) do |exit|
        print "Actor exited with message: #{exit.reason}\n"
        ready_workers << Actor.spawn_link(&work_loop)
      end
    end
    
Mitteilungs-Filter verwenden === für die Mitteilungen, deshalb können auch
reguläre Ausdrücke, Klassen oder proc mit `when()` eingesetzt werden.

Actors können mit Hilfe eines `Actor.spawn_link` auch eine Eltern-Kind Beziehung
haben. Sollte der Kind-Actor aus irgendeinem Grund beendet werden, kann der
Eltern-Actor über die Einstellung `Actor.trap_exit = true` vor dem
`Actor.spawn_link` darüber im weiteren Verlauf benachrichtigt werden. Im
folgenden Beispiel wird ein Supervisor-Actor erstellt, der eine Arbeits-Queue
mit 10 Arbeitern verwaltet. Der Supervisor wird mittels einer
`Actor::DeadActorError` Mitteilung über die Beendigung eines Arbeiters (z.B.
wegen einer Exception) benachrichtigt:

    require 'actor'

    Ready = Struct.new(:this)
    Work = Struct.new(:msg)

    @supervisor = Actor.spawn do
      supervisor = Actor.current
      work_loop = Proc.new do
        loop do
          work = Actor.receive
          puts("Processing: #{work.msg}")
          supervisor << Ready[Actor.current]
        end
      end

      Actor.trap_exit = true
      ready_workers = []
      10.times do |x|
        # starte 10 Arbeiter Actors
        ready_workers << Actor.spawn_link(&work_loop)
      end
      loop do
        Actor.receive do |f|
          f.when(Ready) do |who|
            # SNIP
          end
          f.when(Work) do |work|
            ready_workers.pop << work
          end
          f.when(Actor::DeadActorError) do |exit|
            print "Actor exited with message: #{exit.reason}\n"
            ready_workers << Actor.spawn_link(&work_loop)
          end
        end
      end
    end

    10.times do |idx|
      @supervisor << Work[idx]
    end
    sleep 1

Dieses Beispiel stammt von [girl_friday](http://github.com/mperham/girl_friday).
Weitere Einzelheiten finden sich im Repository.
