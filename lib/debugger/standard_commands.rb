# Defines standard debugger commands that are always available
class Debugger

  class Help < Command
    def help
      return "h[elp]", "Display this list of commands"
    end

    def command_regexp
      /^(?:h(?:elp)?)|\?$/
    end

    def execute(dbg, md)
      output = Output.new
      output << "Available commands:"
      cmds = dbg.commands
      cmds.each do |cmd|
        c,h = cmd.help if cmd.respond_to? :help
        output << [c,h]
      end
      return output
    end
  end


  class ShowBreakpoints < Command
    def help
      return "b[reak]", "List breakpoints"
    end

    def command_regexp
      /^b(?:reak)?$/
    end

    def execute(dbg, md)
      output = Output.new
      output << "Breakpoints:"
      output.set_columns(["%d.", "%s", " %s ", "[IP:%d]"])
      dbg.breakpoints.each_with_index do |bp, i|
        output << [i+1, "#{bp.method.file}:#{bp.line}", bp.method.name, bp.ip]
      end
      output
    end
  end


  class AddBreakpoint < Command
    def help
      return "b[reak] <Method>", "Set a breakpoint at the start of the specified method"
    end

    def command_regexp
      /^b(?:reak)?\s+(?:((?:\w*(?:::)?)*\w+)[.#])?([\w_]+[\w\d?_\[\]])$/
    end

    def execute(dbg, md)
      cls, mthd = md[1], md[2]
      clazz = MAIN
      unless cls.nil?
        clazz = Module.const_get(cls.to_sym)
      end
      cm = clazz.method(mthd.to_sym).compiled_method

      bp = dbg.set_breakpoint cm, 0
      return "Breakpoint set on #{bp.method.name} at #{bp.method.file}:#{bp.line}"
    end
  end


  class Continue < Command
    def help
      return "c[ont]", "Continue execution to next breakpoint, or end of program (whichever comes first)"
    end

    def command_regexp
      /^(?:c(ont(inue)?)?|r(un)?)$/
    end

    def execute(dbg, md)
      dbg.done!
      return "[Resuming program]"
    end
  end


  # Steps to the next line
  class StepNext < Command
    def help
      return "n[ext] [+n]", "Step to the next, (or nth next) line without stepping into called methods"
    end

    def command_regexp
      /^n(?:ext)?(?:\s+(\+)?(\d+))?$/
    end

    def execute(dbg, md)
      # Determine how many lines to step ahead
      step_type = md[1]
      n = md[2]
      steps = nil
      target_line = nil

      # Find IP of line to step to
      ctxt = dbg.debug_context
      cm = ctxt.method
      bc = cm.decode

      if n.nil?
        steps = 1
      elsif step_type.nil?
        target_line = n.to_i
      else
        steps = n.to_i
        steps = 1 if steps < 1
      end

      # Locate the line at n steps past the current line
      bp_ip = nil
      bc.each do |op|
        if steps
          steps -= 1 if op.line > ctxt.line
          if steps == 0
            bp_ip = op.ip
            break
          end
        else
          # Ensure target_line is valid
          if op.line >= target_line
            bp_ip = op.ip
            break
          end
        end
      end

      if bp_ip
        # Set new breakpoint
        dbg.set_breakpoint cm, bp_ip
        ctxt.reload_method
        output = "Stepping to line:#{cm.line_from_ip(bp_ip)}"
      else
        # Stepping past end of method, so set breakpoint at caller
        ctxt = ctxt.sender
        dbg.set_breakpoint ctxt.method, ctxt.ip
        ctxt.reload_method
        output = "Stepping out to caller (#{ctxt.method.name}:#{ctxt.line}) [IP:#{ctxt.ip}]"
      end

      # Instruct debugger to end session and resume debug thread
      dbg.done!

      return output
    end
  end


  class StepOut < Command
    def help
      return "o[ut]", "Return to caller and then break"
    end

    def command_regexp
      /^o(?:ut)?$/
    end

    def execute(dbg, md)
      # Set a breakpoint immediately following the call site
      ctxt = dbg.debug_context.sender
      if ctxt
        dbg.set_breakpoint ctxt.method, ctxt.ip
        ctxt.reload_method
        output = "Stepping out to caller (#{ctxt.method.name}:#{ctxt.line}) [IP:#{ctxt.ip}]"
      end

      # Let the debugger know to resume execution until next breakpoint
      dbg.done!

      return output
    end
  end


  class Quit < Command
    def help
      return "q[uit]", "Remove all breakpoints and resume program"
    end

    def command_regexp
      /^q(?:uit)?$/
    end

    def execute(dbg, md)
      dbg.remove_breakpoint dbg.breakpoints
      dbg.done!
      return "Exiting debugger"
    end
  end


  class ListSource < Command
    def help
      return "l[ist]   [start [end]]", "List source code lines around breakpoint or between start/end"
    end

    def command_regexp
      /^l(?:ist)?(?:\s+(\d+))?(?:\s+(\d+))?$/
    end

    # Lists source code around the specified line
    def execute(dbg, md)
      first, last = md[1], md[2]
      first = first.to_i if first
      last = last.to_i if last
      file = dbg.debug_context.file.to_s

      lines = source_for(file)
      if lines.nil?
        return "No source code available for #{file}"
      end

      line = dbg.debug_context.method.line_from_ip(dbg.debug_context.ip)
      first = line - 5 unless first
      last = first + 10 unless last
      first, last = last, first if first > last
      first = 1 if first < 1
      last = lines.size if last > lines.size

      output = Output.new
      output << "Source lines [#{first}-#{last}] in #{file}:"
      output.set_columns(['%d:', '%-s'])
      output << [nil, '...'] if first > 1
      first.upto(last) do |l|
        if l == line
          output.set_line_marker
          output.set_color :cyan
        end
        output << [l, lines[l-1].chomp]
        output.set_color(:clear) if l == line
      end
      output << [nil, '...'] if last < lines.size
      output
    end

    def source_for(file)
      if File.exists?(file)
        File.readlines(file)
      end
    end
  end


  class ShowLocals < Command
    def help
      return "v[ars]", "Show local variables and their values"
    end

    def command_regexp
      /^v(ars)?$/
    end

    def execute(dbg, inp)
      cm = dbg.debug_context.method

      # Output locals stored in the method context locals tuple
      locals = cm.local_names
      local_vals = dbg.debug_context.locals
      if local_vals
        output = Output.new
        output << "Local variables for #{cm.name}:"
        output.set_columns(['%-s', '%s', '%-s'])
        0.upto(local_vals.size-1) do |i|
          lvar = locals ? locals[i].inspect : '?'
          output << [lvar, '=>', local_vals.at(i).inspect]
        end
        return output
      else
        return "There are no local variables defined in #{cm.name}"
      end
    end
  end


  class ShowBacktrace < Command
    def help
      return "w[here]", "Show execution backtrace"
    end

    def command_regexp
      /^w(here)?$/
    end

    def execute(dbg, inp)
      bt = Backtrace.backtrace(dbg.debug_context)
      output = Output.new
      output << "Backtrace:"
      output.set_columns(['%s', '%|s', '%-s'])
      bt.frames.each_with_index do |frame,i|
        recv, loc = frame.first, frame.last
        if i == 0
          output.set_line_marker
          output.set_color :green
        elsif loc =~ /kernel/
          output.set_color bt.kernel_color
        elsif loc =~ /\(eval\)/
          output.set_color bt.eval_color
        else
          output.set_color :clear
        end
        output << [recv, 'at', loc]
      end
      output
    end
  end


  # Debugger command for evaluating a Ruby expression
  class EvalExpression < Command
    def initialize
      @expr = ''
    end

    def help
      return "", "Any other command is assumed to be a Ruby expression, and is evaluated."
    end

    def command_regexp
      /\w+/
    end

    # This command should have the very lowest priority, as it evaluates the
    # command as a Ruby expression.
    def <=>(other)
      return 1
    end

    def execute(dbg, md)
      @expr += md.string
      begin
        bind = Binding.setup(dbg.debug_context)
        result = eval(@expr, bind)
        output = Output.new
        output.set_line_marker
        output << result.inspect

        @expr = ''
        dbg.prompt = "\nrbx:debug> "
      rescue SystemExit => e
        raise
      rescue Exception => e
        # Processing may continue with incomplete expressions
        unless SyntaxError === e and e.message =~ /unexpected \$end|unterminated string/
          dbg.handle_exception e
          @expr = ''
          dbg.prompt = "\nrbx:debug> "
        else
          dbg.prompt = "rbx:debug*> "
        end
      end
 
      output
    end
  end
end