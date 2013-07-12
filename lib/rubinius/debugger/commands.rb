require 'rubinius/debugger/display'

class Rubinius::Debugger
  class CommandDescription
    attr_accessor :klass, :patterns, :help, :ext_help

    def initialize(klass)
      @klass = klass
    end

    def name
      @klass.name
    end
  end

  class Command
    include Rubinius::Debugger::Display

    @commands = []

    def self.commands
      @commands
    end

    def self.descriptor
      @descriptor ||= CommandDescription.new(self)
    end

    def self.pattern(*strs)
      Command.commands << self
      descriptor.patterns = strs
    end

    def self.help(str)
      descriptor.help = str
    end

    def self.ext_help(str)
      descriptor.ext_help = str
    end

    def self.match?(cmd)
      descriptor.patterns.include?(cmd)
    end

    def initialize(debugger)
      @debugger = debugger
    end

    def run_code(str)
      @debugger.current_frame.run(str)
    end

    def current_method
      @debugger.current_frame.method
    end

    def current_frame
      @debugger.current_frame
    end

    def variables
      @debugger.variables
    end

    def listen(step=false)
      @debugger.listen(step)
    end

    # ===== Commands =====
    #
    # These classes are in the order they should appear in the help output.
    # As such, they're grouped by similar action.

    class Help < Command
      pattern "help", "h"
      help "Show information about debugger commands"

      def run(args)

        if args and !args.empty?
          klass = Command.commands.find { |k| k.match?(args.strip) }
          if klass
            des = klass.descriptor
            puts "Help for #{des.name}:"
            puts "  Accessed using: #{des.patterns.join(', ')}"
            puts "\n#{des.help}."
            puts "\n#{des.ext_help}" if des.ext_help
          else
            puts "Unknown command: #{args}"
          end
        else
          Command.commands.each do |klass|
            des = klass.descriptor

            puts "%20s: #{des.help}" % des.patterns.join(', ')
          end
        end
      end

    end

    class SetBreakPoint < Command
      pattern "b", "break", "brk"
      help "Set a breakpoint at a point in a method"
      ext_help <<-HELP
The breakpoint must be specified using the following notation:
  Klass[.#]method:line

Thus, to set a breakpoint for the instance method pop in
Array on line 33, use:
  Array#pop:33

To breakpoint on class method start of Debugger line 4, use:
  Debugger.start:4

Conditional breakpoints can be created in this way:
  <breakpoint declaration> if <condition>
The breakpoint will be triggered only when the evaluation of the specified condition returns true.
      HELP

      # provide this method so it can be overriden for other languages wanting to use this debugger
      def match_method(method_identifier)
        /([A-Z]\w*(?:::[A-Z]\w*)*)([.#]|::)([a-zA-Z0-9_\[\]]+[!?=]?)(?:[:](\d+))?(\s+if\s+.*)?/.match(method_identifier)
      end

      def run(args, temp=false)
        m = match_method(args)
        unless m
          error "Unrecognized position: '#{args}'"
          return
        end

        klass_name = m[1]
        which      = m[2]
        name       = m[3]
        line       = m[4] ? m[4].to_i : nil
        condition  = m[5] ? m[5].sub(/\A\s+if\s+/, '') : nil

        begin
          klass = run_code(klass_name)
        rescue NameError
          error "Unable to find class/module: #{m[1]}"
          ask_deferred klass_name, which, name, line
          return
        end

        begin
          if which == "#"
            method = klass.instance_method(name)
          else
            method = klass.method(name)
          end
        rescue NameError
          error "Unable to find method '#{name}' in #{klass}"
          ask_deferred klass_name, which, name, line
          return
        end

        bp = @debugger.set_breakpoint_method args.strip, method, line, condition

        bp.set_temp! if temp

        return bp
      end

      def ask_deferred(klass_name, which, name, line)
        answer = ask "Would you like to defer this breakpoint to later? [y/n] "

        if answer.strip.downcase[0] == ?y
          @debugger.add_deferred_breakpoint(klass_name, which, name, line)
          info "Deferred breakpoint created."
        end
      end

    end

    class SetTempBreakPoint < SetBreakPoint
      pattern "tb", "tbreak", "tbrk"
      help "Set a temporary breakpoint"
      ext_help "Same as break, but the breakpoint is deleted when it is hit"

      def run(args)
        super args, true
      end
    end

    class DeleteBreakpoint < Command
      pattern "d", "del", "delete"
      help "Delete a breakpoint"
      ext_help "Specify the breakpoint by number, use 'info break' to see the numbers"

      def run(args)
        if !args or args.empty?
          error "Please specify which breakpoint by number"
          return
        end

        begin
          i = Integer(args.strip)
        rescue ArgumentError
          error "'#{args}' is not a number"
          return
        end

        @debugger.delete_breakpoint(i)
      end
    end

    class Next < Command
      pattern "n", "next"
      help "Move to the next line or conditional branch"
      ext_help <<-HELP
Attempt to continue execution and stop at the next line. If there is
a conditional branch between the current position and the next line,
execution is stopped within the conditional branch instead.

The optional argument is a number which specifies how many lines to
attempt to skip past before stopping execution.

If the current line is the last in a method, execution is stopped
at the current position of the caller.
      HELP

      def run(args)
        if !args or args.empty?
          step = 1
        else
          step = args.to_i
        end

        if step <= 0
          error "Invalid step count - #{step}"
          return
        end

        step_over_by(step)
        @debugger.listen
      end

      def step_over_by(step)
        f = current_frame

        ip = -1

        exec = f.method
        possible_line = f.line + step
        fin_ip = exec.first_ip_on_line possible_line, f.ip

        if !fin_ip
          return step_to_parent
        end

        set_breakpoints_between(exec, f.ip, fin_ip)
      end

      def step_to_parent
        f = @debugger.frame(current_frame.number + 1)
        unless f
          info "Unable to find frame to step to next"
          return
        end

        exec = f.method
        ip = f.ip

        bp = BreakPoint.for_ip(exec, ip)
        bp.for_step!(f.variables)
        bp.activate

        return bp
      end

      def set_breakpoints_between(exec, start_ip, fin_ip)
        ips = goto_between(exec, start_ip, fin_ip)
        if ips.kind_of? Fixnum
          ip = ips
        else
          one, two = ips
          bp1 = BreakPoint.for_ip(exec, one)
          bp2 = BreakPoint.for_ip(exec, two)

          bp1.paired_with(bp2)
          bp2.paired_with(bp1)

          bp1.for_step!(current_frame.variables)
          bp2.for_step!(current_frame.variables)

          bp1.activate
          bp2.activate

          return bp1
        end

        if ip == -1
          error "No place to step to"
          return nil
        end

        bp = BreakPoint.for_ip(exec, ip)
        bp.for_step!(current_frame.variables)
        bp.activate

        return bp
      end

      def next_interesting(exec, ip)
        pop = Rubinius::InstructionSet.opcodes_map[:pop]

        if exec.iseq[ip] == pop
          return ip + 1
        end

        return ip
      end

      def goto_between(exec, start, fin)
        goto = Rubinius::InstructionSet.opcodes_map[:goto]
        git  = Rubinius::InstructionSet.opcodes_map[:goto_if_true]
        gif  = Rubinius::InstructionSet.opcodes_map[:goto_if_false]

        iseq = exec.iseq

        i = start
        while i < fin
          op = iseq[i]
          case op
          when goto
            return next_interesting(exec, iseq[i + 1]) # goto target
          when git, gif
            return [next_interesting(exec, iseq[i + 1]),
              next_interesting(exec, i + 2)] # target and next ip
          else
            op = Rubinius::InstructionSet[op]
            i += (op.arg_count + 1)
          end
        end

        return next_interesting(exec, fin)
      end

    end

    class StepInto < Next
      pattern "s", "step"
      help "Step into next method call or to next line"
      ext_help <<-HELP
Behaves like next, but if there is a method call on the current line,
execution is stopped in the called method.
      HELP

      def run(args)
        max = step_over_by(1)

        listen(true)

        # We remove the max position breakpoint no matter what
        max.remove! if max

      end
    end

    class NextInstruction < Next
      pattern "ni", "nexti"
      help "Move to the next bytecode instruction"
      ext_help <<-HELP
Continue but stop execution at the next bytecode instruction.

Does not step into send instructions.
      HELP

      def run(args)
        if args and !args.empty?
          step = args.to_i
        else
          step = 1
        end

        exec = current_method
        insn = Rubinius::InstructionSet[exec.iseq[current_frame.ip]]

        next_ip = current_frame.ip + insn.width

        if next_ip >= exec.iseq.size
          step_to_parent
        elsif is_a_goto(exec, current_frame.ip)
          set_breakpoints_between(exec, current_frame.ip, next_ip)
        else
          line = exec.line_from_ip(next_ip)

          bp = BreakPoint.for_ip(exec, next_ip)
          bp.for_step!(current_frame.variables)
          bp.activate
        end

        listen
      end

      def is_a_goto(exec, ip)
        goto = Rubinius::InstructionSet.opcodes_map[:goto]
        git  = Rubinius::InstructionSet.opcodes_map[:goto_if_true]
        gif  = Rubinius::InstructionSet.opcodes_map[:goto_if_false]

        i = exec.iseq[ip]

        case i
        when goto, git, gif
          return true
        end

        return false
      end
    end

    class SetFrame < Command
      pattern "f", "frame"
      help "Make a specific frame in the call stack the current frame"
      ext_help <<-HELP
The argument must be a number corrisponding to the frame numbers reported by
'bt'.

The frame specified is made the current frame.
      HELP

      def run(args)
        unless m = /(\d+)/.match(args)
          error "Invalid frame number: #{args}"
          return
        end

        num = m[1].to_i

        if num >= @debugger.locations.size
          error "Frame #{num} too big"
          return
        end

        @debugger.set_frame(num)

        info current_frame.describe
        @debugger.show_code
      end
    end

    class Continue < Command
      pattern "c", "cont", "continue"
      help "Continue running the target thread"
      ext_help <<-HELP
Continue execution until another breakpoint is hit.
      HELP

      def run(args)
        listen
      end
    end

    class Backtrace < Command
      pattern "bt", "backtrace"
      help "Show the current call stack"
      ext_help <<-HELP
Show the call stack as a simple list.

Passing "-v" will also show the values of all locals variables
in each frame.
      HELP

      def run(args)
        verbose = (args =~ /-v/)

        if m = /(\d+)/.match(args)
          count = m[1].to_i
        else
          count = nil
        end

        info "Backtrace:"

        @debugger.each_frame(current_frame) do |frame|
          return if count and frame.number >= count

          info "%4d %s" % [frame.number, frame.describe]

          if verbose
            frame.local_variables.each do |local|
              info "       #{local} = #{frame.run(local.to_s).inspect}"
            end
          end
        end
      end
    end

    class EvalCode < Command
      pattern "p", "eval"
      help "Run code in the current context"
      ext_help <<-HELP
Run code in the context of the current frame.

The value of the expression is stored into a global variable so it
may be used again easily. The name of the global variable is printed
next to the inspect output of the value.
      HELP

      def run(args)
        @debugger.eval_code(args)
      end
    end

    class Disassemble < Command
      pattern "dis", "disassemble"
      help "Show the bytecode for the current line or method"
      ext_help <<-HELP
Disassemble bytecode for the current method. By default, the bytecode
for the current line is disassembled only.

If the argument is 'all', the entire method is shown as bytecode.
      HELP

      def run(args)
        if args and args.strip == "all"
          section "Bytecode for #{current_frame.method.name}"
          puts current_method.decode
        else
          @debugger.show_bytecode
        end
      end
    end

    class ShowInfo < Command
      pattern "i", "info"
      help "Show information about things"
      ext_help <<-HELP
Subcommands are:
  break, breakpoints, bp: List all breakpoints
      HELP

      def run(args)
        if args
          case args.strip
          when "break", "breakpoints", "bp"
            section "Breakpoints"
            if @debugger.breakpoints.empty?
              info "No breakpoints set"
            end

            @debugger.breakpoints.each_with_index do |bp, i|
              if bp
                info "%3d: %s" % [i+1, bp.describe]
                if bp.has_commands?
                  info "     #{bp.commands}"
                end
                if bp.has_condition?
                  info "     stop only if #{bp.condition}"
                end
              end
            end
          else
            error "Unknown info: '#{args}'"
          end
        else
          error "No info subcommand"
        end
      end
    end

    class SetVariable < Command
      pattern "set"
      help "Set a debugger config variable"
      ext_help <<-HELP
Set a debugger configuration variable. Use 'show' to see all variables.
      HELP

      def run(args)
        var, val = args.split(/\s+/, 2)

        if val
          case val.strip
          when "true", "on", "yes", ""
            val = true
          when "false", "off", "no"
            val = false
          when "nil"
            val = nil
          when /\d+/
            val = val.to_i
          end
        else
          val = true
        end

        info "Set '#{var}' = #{val.inspect}"

        @debugger.variables[var.to_sym] = val
      end
    end

    class ShowVariable < Command
      pattern "show"
      help "Display the value of a variable or variables"
      ext_help <<-HELP
Show debugger variables and user created variables. By default,
shows all variables.

The optional argument is which variable specificly to show the value of.
      HELP

      def run(args)
        if !args or args.strip.empty?
          variables.each do |name, val|
            info "var '#{name}' = #{val.inspect}"
          end

          if @debugger.user_variables > 0
            section "User variables"
            (0...@debugger.user_variables).each do |i|
              str = "$d#{i}"
              val = Rubinius::Globals[str.to_sym]
              info "var #{str} = #{val.inspect}"
            end
          end
        else
          var = args.strip.to_sym
          if variables.key?(var)
            info "var '#{var}' = #{variables[var].inspect}"
          else
            error "No variable set named '#{var}'"
          end
        end

      end
    end

    class Quit < Command
      pattern "quit", "q", "exit", "ex"
      help "Quit the debugger"
      ext_help <<-HELP
Quits your current session and shuts down the complete process
      HELP

      def run(args)
        Process.exit!(1)
      end
    end

    class CommandsList < Command
      pattern "commands", "command"
      help "execute code every time breakpoint is reached"
      ext_help <<-HELP
Set commands to be executed when a breakpoint is hit.
Give breakpoint number as argument after "commands".
With no argument, the targeted breakpoint is the last one set.
The commands themselves follow starting on the next line.
Type a line containing "end" to indicate the end of them.
Give "silent" as the first line to make the breakpoint silent;
then no output is printed when it is hit, except what the commands print.
      HELP

      def run(args)
        bp = @debugger.breakpoints[args[:bp_id] - 1]
        bp.set_commands(args[:code])
      end
    end

    class Condition < Command
      pattern "condition", "cond"
      help "New condition expression on breakpoint N"
      ext_help <<-HELP
Specify breakpoint number N to break only if COND is true.
Usage is `condition N COND', where N is an integer and COND is an
expression to be evaluated whenever breakpoint N is reached.
      HELP

      def run(args)
        bp_id, condition = args.split(/\s+/, 2)
        bp_id = bp_id.to_i

        if @debugger.breakpoints.empty?
          error "No breakpoint set"
          return
        elsif bp_id > @debugger.breakpoints.size || bp_id < 1
          error "Invalid breakpoint number."
          return
        end

        bp = @debugger.breakpoints[bp_id - 1]
        bp.set_condition(condition)
      end
    end

    class ListCode < Command
      pattern "l", "list"
      help "List code"
      ext_help <<-HELP
List specified function or line.
With no argument, lists ten more lines after or around previous listing.
"list -" lists the ten lines before a previous ten-line listing.
One argument specifies a line, and ten lines are listed around that line.
Two arguments with comma between specify starting and ending lines to list.
Lines can be specified in these ways:
  LINENUM, to list around that line in current file,
  FILE:LINENUM, to list around that line in that file,
      HELP

      def run(args)
        path         = nil
        line         = nil
        lines_around = 10

        if args =~ /^[\w#{File::Separator}]+(\.rb)?:\d+$/
          path, line = args.split(':')
          line = line.to_i
        elsif args.nil?
          line = if @debugger.variables[:list_command_history][:center_line]
            @debugger.variables[:list_command_history][:center_line] + 1 + lines_around
          else
            @debugger.current_frame.line.to_i
          end
          path = @debugger.variables[:list_command_history][:path] || @debugger.current_frame.method.active_path
        elsif args == "-"
          if @debugger.variables[:list_command_history][:center_line].nil? || @debugger.variables[:list_command_history][:path].nil?
            return
          else
            line = @debugger.variables[:list_command_history][:center_line] - lines_around
            path = @debugger.variables[:list_command_history][:path]
          end
        elsif args =~ /^\d+$/
          line = args.to_i
          path = @debugger.current_frame.method.active_path
        elsif match = /^(\d+),(\d+)$/.match(args)
          start_line = match[1].to_i
          end_line   = match[2].to_i
          path       = @debugger.current_frame.method.active_path

          @debugger.list_code_range(path, start_line, end_line, end_line)
          return
        else
          error 'Invalid args for list'
          return
        end

         @debugger.list_code_around_line(path, line, lines_around)
      end
    end
  end

end
