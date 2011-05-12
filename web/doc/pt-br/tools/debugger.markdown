---
layout: doc_pt_br
title: Debugger
previous: Tools
previous_url: tools
next: Profiler
next_url: tools/profiler
---

Rubinius includes a reference Ruby source-level debugger.

## Invoking in Code

The Rubinius debugger can be invoked from code by embedding a call to start
the debugger

Consider the following code in app.rb

    class Toast
      attr_accessor :setting
      def initialize
        require 'rubinius/debugger'
        Rubinius::Debugger.start
        @setting = :brown
      end
    end

    p Toast.new.setting

Running the code in Rubinius would produce the following:

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

As shown, help for the debugger commands is available by typing `help` when in
the debugger.


## Invoking from the Command Line

The Rubinius debugger can be invoked from the command line and will present
the debugger prompt just before loading the script specified on the command
line.

Consider the following Ruby code in bug.rb:

    def problem_code
      puts "I have a problem"
      a = 1 + 2
      puts "a math problem" unless a == 4
    end

    problem_code

To debug this code, start the debugger from the command line with the
`-Xdebug` option.

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

As you can see, executing `p a = 4` in the debugger changes the value of the
local variable `a` and affects the execution of the Ruby code.
