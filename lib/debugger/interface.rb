class Debugger
  class Interface
    def process_commands(dbg, thread, ctxt, bp_list)
      raise "Not implemented!"
    end

    def handle_exception(e)
      raise e
    end
  end


  # Simple command-line interface to the Debugger
  class CmdLineInterface < Interface
    def initialize(out=STDOUT, err=STDERR)
      # HACK readline causes `rake spec` to hang in ioctl()
      require 'readline-native'
      @out, @err = out, err
    end

    # Activates the debugger after a breakpoint has been hit, and responds to
    # debgging commands until a continue command is recevied.
    def process_commands(dbg, thread, ctxt, bp_list)
      bp = bp_list.last
      file = ctxt.file.to_s
      line = ctxt.method.line_from_ip(ctxt.ip)
      @out.puts "[Debugger activated]" unless bp.kind_of? StepBreakpoint
      @out.puts ""
      @out.puts "#{file}:#{line} (#{ctxt.method.name}) [IP:#{ctxt.ip}]"

      # Display current line/instruction
      output = Output.new
      output.set_line_marker
      output.set_color :cyan
      if bp.kind_of? StepBreakpoint and bp.step_by == :ip
        bc = ctxt.method.decode
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

      until dbg.done? do
        if dbg.more_input?
          @input_line += 1
          prompt = "rbx:debug:#{@input_line}> "
        else
          @input_line = 1
          prompt = "\nrbx:debug> "
        end
        inp = Readline.readline(prompt)
        inp.strip!
        @last_inp = inp if inp.length > 0
        output = dbg.process_command(@last_inp)
        @out.puts output if output
      end
      @out.puts "[Debugger exiting]" if dbg.quit?
    end

    # Handles any exceptions raised by a Command subclass during a debug session.
    def handle_exception(e)
      @err.puts ""
      @err.puts "An exception has occurred:\n    #{e.message} (#{e.class})"
      @err.puts "Backtrace:"
      output = Output.new
      output.set_columns(['%s', '%-s'], ' at ')
      bt = e.awesome_backtrace
      first = true
      bt.frames.each do |fr|
        recv = fr[0]
        loc = fr[1]
        break if recv =~ /Debugger#process_command/
        output.set_color(bt.color_from_loc(loc, first))
        first = false # special handling for first line
        output << [recv, loc]
      end
      @err.puts output
    end
  end
end