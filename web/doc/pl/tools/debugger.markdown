---
layout: doc_pl
title: Debugger
previous: Narzędzia
previous_url: tools
next: Profiler
next_url: tools/profiler
---

Rubinius zawiera referencyjny debugger na poziomie kodu źródłowego.

## Wywołanie debuggera

Debugger można wywołać z poziomu kodu źródłowego programu wykonując Rubinius::Debugger.start

Biorąc za przykład kod z app.rb

    class Toast
      attr_accessor :setting
      def initialize
        require 'rubinius/debugger'
        Rubinius::Debugger.start
        @setting = :brown
      end
    end

    p Toast.new.setting

Wykonanie powyższego kodu w Rubiniusie spowoduje:

    $ rbx app.rb

    | Breakpoint: Toast#initialize at app.rb:5 (15)
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
    |    0 Toast#initialize at app.rb:5 (15)
    |    1 main.__script__ at app.rb:11 (46)
    |    2 Rubinius::CodeLoader#load_script(debug) at kernel/delta/codeloader.rb:67 (44)
    |    3 Rubinius::CodeLoader.load_script(name) at kernel/delta/codeloader.rb:91 (40)
    |    4 Rubinius::Loader#script at kernel/loader.rb:460 (79)
    |    5 Rubinius::Loader#main at kernel/loader.rb:571 (64)
    |    6 Rubinius::Loader.main at kernel/loader.rb:609 (33)
    |    7 Object#__script__ at kernel/loader.rb:621 (60)
    debug> n

    | Breakpoint: Toast#initialize at app.rb:6 (16)
    | 6:     @setting = :brown
    debug> n

    | Breakpoint: Toast#initialize at app.rb:7 (21)
    | 7:     @kind = :rye
    debug> p @setting = :light_brown
    $d0 = :light_brown
    debug> c
    :light_brown

Opis wszystkich komend uzyskamy wpisując `help` w konsoli debuggera.


## Wywołanie debuggera z linii poleceń

Debugger można wywołać z linii poleceń. Debugowanie rozpocznie się
od pierwszej linii kodu wywoływanego skryptu.

Zakładając poniższy kod w skrypcie bug.rb:

    def problem_code
      puts "I have a problem"
      a = 1 + 2
      puts "a math problem" unless a == 4
    end

    problem_code

Aby zdebugować powyższy kod uruchom Rubiniusa z opcją `-Xdebug`.

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
    | 2:   puts "I have a problem"
    debug> n
    I have a problem

    | Breakpoint: Object#problem_code at bug.rb:3 (9)
    | 3:   a = 1 + 2
    debug> n

    | Breakpoint: Object#problem_code at bug.rb:4 (16)
    | 4:   puts "a math problem" unless a == 4
    debug> p a = 4
    $d0 = 4
    debug> n

    | Breakpoint: main.__script__ at bug.rb:7 (17)
    | 7: problem_code
    debug> c

Jak widać, wywołanie `p a = 4` w debugerze zmienia wartość zmiennej
lokalnej `a` oraz wpływa na wykonywany program.
