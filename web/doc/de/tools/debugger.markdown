---
layout: doc_de
title: Debugger
previous: Tools
previous_url: tools
next: Profiler
next_url: tools/profiler
---

Rubinius kommt mit einer eingebauten Ruby Debugger-API auf Quell-Ebene.

## Aus aktivem Code heraus starten

Der Rubinius Debugger kann direkt aus aktivem Code heraus mit Hilfe eines
eingebauten Aufrufs gestartet werden.

Angenommen folgender Code befindet sich in app.rb:

    class Toaster
      attr_accessor :einstellung
      def initialize
        require 'rubinius/debugger'
        Rubinius::Debugger.start
        @einstellung = :braun
      end
    end

    p Toaster.new.einstellung

Bei Ausführung des Codes in Rubinius käme folgende Ausgabe:

    $ rbx app.rb

    | Breakpoint: Toaster#initialize at app.rb:5 (15)
    | 5:     Rubinius::Debugger.start
    debug> help
                    help: Show information about debugger commands
           b, break, brk: Set a breakpoint at a point in a method
        tb, tbreak, tbrk: Set a temporary breakpoint
               d, delete: Delete a breakpoint
                 n, next: Move to the next line or conditional branch
                 s, step: Step into next method call or to next line
               ni, nexti: Move to the next bytecode instruction
                f, frame: Make a specific frame in the call stack the current frame
       c, cont, continue: Continue running the target thread
           bt, backtrace: Show the current call stack
                 p, eval: Run code in the current context
        dis, disassemble: Show the bytecode for the current method
                 i, info: Show information about things
                     set: Set a debugger config variable
                    show: Display the value of a variable or variables
    debug> bt
    | Backtrace:
    |    0 Toaster#initialize at app.rb:5 (15)
    |    1 main.__script__ at app.rb:11 (46)
    |    2 Rubinius::CodeLoader#load_script(debug) at kernel/delta/codeloader.rb:67 (44)
    |    3 Rubinius::CodeLoader.load_script(name) at kernel/delta/codeloader.rb:91 (40)
    |    4 Rubinius::Loader#script at kernel/loader.rb:460 (79)
    |    5 Rubinius::Loader#main at kernel/loader.rb:571 (64)
    |    6 Rubinius::Loader.main at kernel/loader.rb:609 (33)
    |    7 Object#__script__ at kernel/loader.rb:621 (60)
    debug> n

    | Breakpoint: Toaster#initialize at app.rb:6 (16)
    | 6:     @einstellung = :braun
    debug> n

    | Breakpoint: Toaster#initialize at app.rb:7 (21)
    | 7:     @art = :roggen
    debug> p @einstellung = :hellbraun
    $d0 = :hellbraun
    debug> c
    :hellbraun

Wie angegeben, bekommt man bei der Eingabe von `help` im Debugger eine
englischsprachige Hilfsanzeige für die verfügbaren Befehle.


## Starten von der Kommandozeile

Der Rubinius Debugger kann ebenfalls aus der Kommandozeile heraus gestartet
werden und zeigt das Debugger Prompt an, noch bevor das dazu angegebene Skript
geladen wird.

Angenommen folgender Code befindet sich in app.rb:

    def problem_code
      puts "Ich habe ein Problem"
      a = 1 + 2
      puts "ein Mathe-Problem" unless a == 4
    end

    problem_code

Um diesen Code zu debuggen, kann der Debugger von der Kommandozeile aus mit der
`-Xdebug` option gestartet werden:

    $ rbx -Xdebug bug.rb

    | Breakpoint: Rubinius::Loader#debugger at kernel/loader.rb:424 (34)
    | 424:         Rubinius::Debugger.start
    debug> b Object#problem_code:2
    * Unable to find method 'problem_code' in Object
    | Would you like to defer this breakpoint to later? [y/n] y
    | Defered breakpoint created.
    debug> c
    | Resolved breakpoint for Object#problem_code
    | Set breakpoint 2: bug.rb:2 (+0)

    | Breakpoint: Object#problem_code at bug.rb:2 (0)
    | 2:   puts "Ich habe ein Problem"
    debug> n
    I habe ein Problem

    | Breakpoint: Object#problem_code at bug.rb:3 (9)
    | 3:   a = 1 + 2
    debug> n

    | Breakpoint: Object#problem_code at bug.rb:4 (16)
    | 4:   puts "ein Mathe-Problem" unless a == 4
    debug> p a = 4
    $d0 = 4
    debug> n

    | Breakpoint: main.__script__ at bug.rb:7 (17)
    | 7: problem_code
    debug> c

Wie man sieht, bei der Ausführung von `p a = 4` im Debugger wird der Wert der
lokalen Variable `a` geändert und beeinflusst damit auch die weitere Ausführung
des Ruby-Codes.
