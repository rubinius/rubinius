require 'debugger/debugger'
require 'debugger/command'
require 'debugger/output'

module Rubinius
  class Debugger
    class Interface
      # The current thread and context being debugged
      attr_reader :debug_thread, :debug_context

      # The current eval context, i.e. the context in which most commands will be
      # executed. By default, this is the same as the debug context, but it can be
      # changed via the Up/Down commands.
      attr_accessor :eval_context

      def process_commands(dbg, thread, ctxt, bp_list)
        raise "Not implemented!"
      end

      def handle_exception(e)
        raise e
      end

      # (Re-)loads the available commands from all registered sub-classes of
      # Debugger::Command.
      def load_commands
        @commands = Debugger::Command.available_commands.map do |cmd_class|
          cmd_class.new
        end
        @commands.sort!
      end

      # Returns the available debugger commands, which are instances of all loaded
      # Debugger::Command subclasses.
      def commands
        load_commands unless @commands
        @commands
      end

      def at_breakpoint(dbg, thread, ctxt, bp_list)
        @debug_thread = thread
        @eval_context = @debug_context = ctxt
        @done = false

        process_commands(dbg, thread, ctxt, bp_list)

        # Clear any references to the debuggee thread and context
        @debug_thread = nil
        @debug_context = nil
        @eval_context = nil
      end

      # Sets the done flag to true, so that the debugger resumes the debug thread.
      def done!
        @done = true
      end

      # Returns true if execution is about to be resumed
      def done?
        @done
      end

      # Processes a debugging command by finding a Command subclass that can handle
      # the input, and delegating to it.
      def process_command(dbg, inp)
        if @more_input and @last_command
          @more_input = false
          output = @last_command.execute(self, inp)
        else
          @more_input = false
          @commands.each do |cmd|
            if inp =~ cmd.command_regexp
              begin
                @last_command = cmd.multiline? ? cmd : nil
                output = cmd.execute(dbg, self, $~)
              rescue StandardError => e
                handle_exception e
              end
              break
            end
          end
        end
        output
      end

      # Flags to the debugger that a command requires more input
      def more_input!
        @more_input = true
      end

      def more_input?
        @more_input
      end
    end


    # Simple command-line interface to the Debugger
    class CmdLineInterface < Interface
      def initialize(out=STDOUT, err=STDERR)
        require 'readline'
        @out, @err = out, err
        load_commands
        Debugger.instance.interface = self
      end

      # Activates the debugger after a breakpoint has been hit, and responds to
      # debgging commands until a continue command is recevied.
      def process_commands(dbg, thread, ctxt, bp_list)
        bp = bp_list.last
        file = ctxt.file.to_s
        line = ctxt.method.line_from_ip(ctxt.ip)
        if bp.kind_of? StepBreakpoint
          output = Output.new
        else
          output = Output.info "[Debugger activated]", :yellow
        end
        output << "#{file}:#{line} (#{ctxt.method.name}) [IP:#{ctxt.ip}]"

        # Display current line/instruction
        output.set_line_marker
        output.set_color :cyan
        if bp.kind_of? StepBreakpoint and bp.step_by == :ip
          bc = dbg.asm_for(ctxt.method)
          inst = bc[bc.ip_to_index(ctxt.ip)]
          output.set_columns(["%04d:", "%-s ", "%-s"])
          output << [inst.ip, inst.opcode, inst.args.map{|a| a.inspect}.join(', ')]
        else
          lines = dbg.source_for(file)
          unless lines.nil?
            output.set_columns(['%d:', '%-s'])
            output << [line, lines[line-1].chomp]
          end
        end
        output.set_color :clear
        @out.puts output

        until @done do
          # Determine prompt
          if more_input?
            @input_line += 1
            prompt = "rbx:debug:#{@input_line}> "
          else
            @input_line = 1
            prompt = "\nrbx:debug> "
          end
          # Read command
          inp = Readline.readline(prompt)
          if inp.nil?
            @done = true
            break
          end
          inp.strip!
          @last_inp = inp if inp.length > 0
          output = process_command(dbg, @last_inp)
          # Display output (if any)
          if output
            if @out == STDOUT
              page_size, page_width = `stty size`.split(' ').map{|s| s.to_i}
              lines = output.lines(page_width)
              i = 0
              while i < lines.size
                @out.puts lines[i...(i+page_size-1)].join("\n")
                i += page_size - 1
                if i < lines.size
                  @out.write "-- More --"
                  # TODO: Fix this when getc doesn't wait for enter to be pressed
                  key = STDIN.gets.strip
                  break if key.downcase == "q"
                end
              end
            else
              @out.puts output
            end
          end
        end

        if dbg.quit?
          output = Output.info "[Debugger exiting]", :yellow
          @out.puts output
        end
      end

      # Handles any exceptions raised by a Command subclass during a debug session.
      def handle_exception(e)
        @err.puts ""
        @err.puts "An exception has occurred:\n    #{e.message} (#{e.class})"
        @err.puts "Backtrace:"
        bt = e.awesome_backtrace
        begin
          output = Output.new
          output.set_columns(['%s', '%-s'], ' at ')
          first = true
          bt.frames.each do |ctxt|
            recv = ctxt.describe
            loc = ctxt.location
            break if recv =~ /Debugger.*#process_command/
            output.set_color(bt.color_from_loc(loc, first))
            first = false # special handling for first line
            output << [recv, loc]
          end
        rescue
          # Can't process the backtrace for some reason - just fallback to std
          output = bt.show
        end
        @err.puts output
      end
    end
  end
end

