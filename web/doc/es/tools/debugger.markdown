---
layout: doc_es
title: Depurador
previous: Herramientas
previous_url: tools
next: Analizador
next_url: tools/profiler
translated: false
---

Rubinius incluye depurador de código fuente Ruby para usar como referencia.

## Invocar en el Código

El depurador de Rubinius puede ser invocado desde el código fuente escribiendo
una llamada para iniciar el depurador.

Considere el siguiente código en app.rb

    class Toast
      attr_accessor :setting
      def initialize
        require 'rubinius/debugger'
        Rubinius::Debugger.start
        @setting = :brown
      end
    end

    p Toast.new.setting

Al ejecutar el código en Rubinius produciría lo siguiente:

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

Como se ve, la ayuda para los comandos de depuración se puede acceder
escribiendo 'help'.

## Invocación desde la línea de comandos

El depurador de Rubinius se puede invocar desde la línea de comandos y
presentará el depurador del sistema justo antes de cargar el script
especificado en la línea de comandos.

Considere el siguiente código de Ruby en bug.rb:

    def problem_code
      puts "I have a problem"
      a = 1 + 2
      puts "a math problem" unless a == 4
    end

    problem_code

Para depurar el código, inicie el depurador de la línea de comandos con la
opción `--Xdebug`.

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

Como puede ver, la ejecución de `p a = 4` en el depurador cambia el valor
de la variable local `a` y afecta la ejecución del código Ruby.
