# Defines standard debugger commands that are always available
class Debugger

  class Help < Command
    def help
      return "h[elp]", "Display this list of commands."
    end

    def command_regexp
      /^(?:(?:h(?:elp)?)|\?)$/
    end

    def execute(dbg, interface, md)
      output = Output.info("Available commands:")
      output.set_columns(["%-s", "%-*s"], '  ')
      cmds = interface.commands
      cmds.each do |cmd|
        c,h = cmd.help if cmd.respond_to? :help
        output << [c,h]
      end
      return output
    end
  end


  class ShowBreakpoints < Command
    def help
      return "b[reak]", "List breakpoints."
    end

    def command_regexp
      /^b(?:reak)?$/
    end

    def execute(dbg, interface, md)
      if bp_list = dbg.breakpoints and bp_list.size > 0
        output = Output.info("Breakpoints:")
        output.set_columns(["%d.", ["File:line", "%-*s"], ["IP", "%d"],
                           ["Method", " %s "], ["Condition", "%*s"],
                           ["Hits", "%4d"], "%s"])
        dbg.breakpoints.each_with_index do |bp, i|
          if bp.enabled?
            output.set_color :white
          else
            output.set_color :yellow
          end
          output << [i+1, "#{bp.method.file}:#{bp.line}", bp.ip, bp.method.name,
                     bp.condition, bp.hits, "#{'(disabled)' unless bp.enabled?}"]
        end
      else
        output = Output.none("No breakpoints currently defined")
      end
      output
    end
  end


  class AddBreakpoint < Command
    @@re = Regexp.new('^b(?:reak)?\s+' + MODULE_METHOD_RE + '(?::(\d+))?(?:\s+((?:if|unless).+))?$')

    def help
      return "b[reak] <method>[:<line>] [if|unless <expr>]", 
              "Set a breakpoint at the start or specified line of <method>."
    end

    def command_regexp
      @@re
    end

    def execute(dbg, interface, md)
      mod, mthd_type, mthd, line, cond = md[1], md[2], md[3], md[4], md[5]
      cm = get_method(mod, mthd_type, mthd).compiled_method
      ip = 0
      ip = cm.first_ip_on_line(line.to_i) if line

      bp = dbg.set_breakpoint cm, ip, cond
      Output.info "Breakpoint set on #{bp.method.name} at #{bp.method.file}:#{bp.line}"
    end
  end


  class AddBreakpointInFile < Command
    @@re = Regexp.new('^b(?:reak)?\s+([^:]+):(\d+)(?:\s+((?:if|unless).+))?$')

    def help
      return "b[reak] <file>:<line> [if|unless <expr>]", 
              "Set a breakpoint on the specified line of <file>."
    end

    def command_regexp
      @@re
    end

    def execute(dbg, interface, md)
      file, line, cond = md[1], md[2].to_i, md[3]
      if cm = CompiledMethod.script_for_file(file)
        cm, ip = cm.locate_line(line)
      else
        return Output.error("No loaded file found matching #{file}")
      end

      if cm
        bp = dbg.set_breakpoint(cm, ip, cond)
        Output.info("Breakpoint set on #{bp.method.name} at #{bp.method.file}:#{bp.line}")
      else
        Output.error("Cannot set a breakpoint on line #{line} of #{file}")
      end
    end
  end


  class DeleteBreakpoint < Command
    def help
      return "b[reak] d[el] <n>", "Delete breakpoint <n>."
    end

    def command_regexp
      /^b(?:reak)?\s+d(?:el(?:ete)?)?\s+(\d+)$/
    end

    def execute(dbg, interface, md)
      n = md[1].to_i
      bp = dbg.breakpoints[n-1]
      if bp
        dbg.remove_breakpoint bp
        Output.info("Deleted breakpoint #{n} from #{bp.method.file}:#{bp.line}")
      else
        Output.error("No such breakpoint")
      end
    end
  end


  class EnableBreakpoint < Command
    def help
      return "b[reak] (en|dis)[able] <n>", "Enable or disable breakpoint <n>."
    end

    def command_regexp
      /^b(?:reak)?\s+(en|dis)(?:able)?\s+(\d+)$/
    end

    def execute(dbg, interface, md)
      enable = md[1] == "en"
      n = md[2].to_i
      bp = dbg.breakpoints[n-1]
      if bp
        if enable
          dbg.enable_breakpoint(bp)
          Output.info("Enabled breakpoint #{n} at #{bp.method.file}:#{bp.line}")
        else
          dbg.disable_breakpoint(bp)
          Output.info("Disabled breakpoint #{n} at #{bp.method.file}:#{bp.line}")
        end
      else
        Output.error("No such breakpoint")
      end
    end
  end


  class Continue < Command
    def help
      return "c[ont]", "Continue execution to next breakpoint, or end of program (whichever comes first)."
    end

    def command_regexp
      /^(?:c(ont(inue)?)?|r(un)?)$/
    end

    def execute(dbg, interface, md)
      interface.done!
      Output.info "[Resuming program]", :yellow
    end
  end


  # Step in to the next line
  class StepIn < Command
    def help
      return "s[tep] [to] [n]", "Step to the next, (or nth next, or specified) line, stepping into called methods."
    end

    def command_regexp
      /^s(?:tep)?(?:\s+(?:(to)\s+)?(\d+))?$/
    end

    def execute(dbg, interface, md)
      step_type = md[1]
      n = md[2]
      steps = nil

      selector = {:step_type => :in, :step_by => :line}
      if step_type
        selector[:target] = n.to_i
        output = Output.info("Stepping to line #{n}")
      else
        n = selector[:steps] = n ? n.to_i : 1
        output = Output.info("Stepping #{n} line#{'s' unless n.to_i == 1}")
      end
      dbg.step(selector)

      # Instruct debugger to end session and resume debug thread
      interface.done!

      return output
    end
  end

  # Step next to the next line
  class StepNext < Command
    def help
      return "n[ext] [to] [n]", "Step to the next, (or nth next, or specified) line, without stepping into called methods."
    end

    def command_regexp
      /^n(?:ext)?(?:\s+(?:(to)\s+)?(\d+))?$/
    end

    def execute(dbg, interface, md)
      step_type = md[1]
      n = md[2]
      steps = nil

      selector = {:step_type => :next, :step_by => :line}
      if step_type
        selector[:target] = n.to_i
        output = Output.info("Stepping to line #{n}")
      else
        n = selector[:steps] = n ? n.to_i : 1
        output = Output.info("Stepping #{n} line#{'s' unless n.to_i == 1}")
      end
      dbg.step(selector)

      # Instruct debugger to end session and resume debug thread
      interface.done!

      return output
    end
  end

  # Step out to caller
  class StepOut < Command
    def help
      return "o[ut] [n]", "Step out to the calling method (or nth caller)."
    end

    def command_regexp
      /^o(?:ut)?(?:\s+(\d+))?$/
    end

    def execute(dbg, interface, md)
      step_type = md[1]
      n = md[1]
      steps = nil

      selector = {}
      n = selector[:out] = n ? n.to_i : 1
      output = Output.info("Stepping out #{n} frame#{'s' unless n.to_i == 1}")
      dbg.step(selector)

      # Instruct debugger to end session and resume debug thread
      interface.done!

      return output
    end
  end


  class Quit < Command
    def help
      return "q[uit]", "Remove all breakpoints, quit the debugger, and resume program."
    end

    def command_regexp
      /^q(?:uit)?$/
    end

    def execute(dbg, interface, md)
      interface.done!
      dbg.quit!
    end
  end


  class ListSource < Command
    @@re = Regexp.new('^l(?:ist)?(?:\s+' + MODULE_METHOD_RE + ')?(?:\s+(\d+))?(?:(?:\s+|-)(\d+))?$')

    def help
      return "l[ist] [<method>] [start [end]]", "List source code lines around breakpoint or <method>, and between start/end."
    end

    def command_regexp
      @@re
    end

    # Lists source code around the specified line
    def execute(dbg, interface, md)
      mod, mthd_type, mthd = md[1], md[2], md[3]
      first, last = md[4], md[5]

      if mthd
        cm = get_method(mod, mthd_type, mthd).compiled_method
        ip = 0
        last = cm.lines.last.last + 1 unless last
      else
        # Decode current method
        cm = interface.eval_context.method
        ip = interface.eval_context.ip
      end

      file = cm.file.to_s
      lines = dbg.source_for(file)
      if lines.nil?
        return Output.error("No source code available for #{file}")
      end

      first = first.to_i if first
      last = last.to_i if last
      line = cm.line_from_ip(ip)
      unless first
        first = line
        first -= 10 unless mthd
      end
      last = first + 20 unless last
      first, last = last, first if first > last
      first = 1 if first < 1
      last = lines.size if last > lines.size

      output = Output.info("Source lines [#{first}-#{last}] in #{file}:")
      output.set_columns(['%d:', '%-*s'])
      output << [nil, '...'] if first > 1
      first.upto(last) do |l|
        if !mthd and l == line
          output.set_line_marker
          output.set_color :cyan
        end
        output << [l, lines[l-1].chomp]
        output.set_color(:clear) if l == line
      end
      output << [nil, '...'] if last < lines.size
      output
    end
  end


  class ListSexp < Command
    @@re = Regexp.new('^sexp(?:\s+' + MODULE_METHOD_RE + ')?$')

    def help
      return "sexp [<method>]", "List S-expression for current or specified method."
    end

    def command_regexp
      @@re
    end

    # Lists source code around the specified line
    # TODO: Change to stored Sexp on CompiledMethod once this is available
    def execute(dbg, interface, md)
      mod, mthd_type, mthd = md[1], md[2], md[3]

      if mthd
        cm = get_method(mod, mthd_type, mthd).compiled_method
      else
        # Decode current method
        cm = interface.eval_context.method
      end
      first = cm.lines.first.last - 1
      last = cm.lines.last.last + 1

      file = cm.file.to_s
      lines = dbg.source_for(file)
      if lines.nil?
        return Output.error("No source code available for #{file}")
      end

      output = Output.info("S-expression for source lines [#{first+1}-#{last+1}] in #{file}:")
      sexp = lines[first..last].join().to_sexp.indented_inspect
      output << sexp
      output
    end
  end


  class ShowLocals < Command
    def help
      return "[l]v[ars]", "Show local variables and their values."
    end

    def command_regexp
      /^l?v(?:ars)?$/
    end

    def execute(dbg, interface, inp)
      cm = interface.eval_context.method

      # Output locals stored in the method context locals tuple
      locals = cm.local_names
      local_vals = interface.eval_context.locals
      if local_vals and local_vals.size > 1 or 
          (local_vals.size == 0 and local_vals.at(0).to_s[0] != ?@)
        output = Output.info("Local variables for #{cm.name}:")
        output.set_columns([['Name', '%-s'], ['Class', '%-s'], ['Value', '%-*s']])
        0.upto(local_vals.size-1) do |i|
          lvar = locals ? locals[i].to_s : '?'
          val = local_vals.at(i)
          output << [lvar, val.class, val.inspect] unless lvar[0] == ?@
        end
        output
      else
        Output.none("There are no local variables defined in #{cm.name}")
      end
    end
  end


  class ShowGlobals < Command
    def help
      return "gv[ars]", "Show global variables and their values."
    end

    def command_regexp
      /^gv(?:ars)?$/
    end

    def execute(dbg, interface, inp)
      # Output globals
      output = Output.info("Global variables:")
      output.set_columns([['Name', '%-s'], ['Class', '%-s'], ['Value', '%-*s']])
      Globals.variables.dup.sort{|a,b| a.to_s <=> b.to_s}.each do |v|
        val = Globals[v]
        output << [v, val.class, val.inspect]
      end
      output
    end
  end


  class ShowIVars < Command
    def help
      return "iv[ars]", "Show instance variables and their values."
    end

    def command_regexp
      /^iv(?:ars)?$/
    end

    def execute(dbg, interface, inp)
      cm = interface.eval_context.method

      # Output ivars on the current self
      bind = Binding.setup(interface.eval_context)
      instance = eval("self", bind)
      ivars = instance.instance_variables
      if ivars.size > 0
        output = Output.info("Instance variables for #{instance}:")
        output.set_columns([['Name', '%-s'], ['Class', '%-s'], ['Value', '%-*s']])
        ivars.each do |ivar|
          val = instance.instance_variable_get(ivar)
          output << [ivar, val.class, val.inspect]
        end
        output
      else
        Output.none("There are no instance variables defined for #{instance}")
      end
    end
  end


  class ShowCVars < Command
    @@re = Regexp.new('^cv(?:ars)?(?:\s+('+ MODULE_RE+ '))?$')

    def help
      return "cv[ars] [<class>]", "Show class variables and their values for the specified class, or current self class."
    end

    def command_regexp
      @@re
    end

    def execute(dbg, interface, md)
      cm = interface.eval_context.method

      # Output ivars on the specified class current self
      bind = Binding.setup(interface.eval_context)
      klass = eval(md[1] || "self", bind)
      cvars = klass.class_variables
      
      if cvars.size > 0
        output = Output.info("Class variables for #{klass}:")
        output.set_columns([['Name', '%-s'], ['Class', '%-s'], ['Value', '%-*s']])
        cvars.each do |cvar|
          val = klass.class_variable_get(cvar)
          output << [cvar, val.class, val.inspect]
        end
        output
      else
        Output.none("There are no class variables defined for #{klass}")
      end
    end
  end


  class ShowBacktrace < Command
    def help
      return "w[here] | b[ack]t[race]", "Show execution backtrace."
    end

    def command_regexp
      /^w(?:here)?|b(?:ack)?t(?:race)?$/
    end

    def execute(dbg, interface, inp)
      bt = Backtrace.backtrace(interface.debug_context)
      eval_ctxt = interface.eval_context
      output = Output.info("Backtrace:")
      output.set_columns(['%*s', '%|s', '%-*s'])
      bt.frames.each_with_index do |frame,i|
        output.set_line_marker if frame == eval_ctxt
        recv, loc = frame.describe, frame.location
        if i == 0
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
      return "<expr>", "Any other command is assumed to be a Ruby expression, and is evaluated."
    end

    def command_regexp
      /\w+/
    end

    # Eval commands can extend over more than one line
    def multiline?
      true
    end

    # This command should have the very lowest priority, as it evaluates the
    # command as a Ruby expression.
    def <=>(other)
      return 1
    end

    def execute(dbg, interface, md)
      @expr += md.kind_of?(MatchData) ? md.string : md
      @expr += "\n"
      begin
        bind = Binding.setup(interface.eval_context)
        result = eval(@expr, bind)
        output = Output.new
        output.set_line_marker
        output << result.inspect

        @expr = ''
      rescue SystemExit => e
        raise
      rescue Exception => e
        # Processing may continue with incomplete expressions
        unless SyntaxError === e and e.message =~ /unexpected \$end|unterminated string/
          interface.handle_exception e
          @expr = ''
        else
          interface.more_input!
        end
      end
 
      output
    end
  end


  class UpFrame < Command
    def help
      return "up [n]", "Change the eval context by moving up 1 (or n) call frames."
    end

    def command_regexp
      /^up(?:\s+(\d+))?$/
    end

    def execute(dbg, interface, md)
      n = (md[1] or "1").to_i
      ctxt = interface.eval_context
      n.downto(1) { ctxt = ctxt.sender if ctxt.sender }
      interface.eval_context = ctxt
      output = Output.info("Evaluation context now at:")
      output.set_columns(['%*s', '%|s', '%-*s'])
      output.set_line_marker
      loc = ctxt.location
      bt = Backtrace.new
      if ctxt == interface.debug_context
        output.set_color :green
      elsif loc =~ /kernel/
        output.set_color bt.kernel_color
      elsif loc =~ /\(eval\)/
        output.set_color bt.eval_color
      end
      output << [ctxt.describe, 'at', loc]
      output
    end
  end


  class DownFrame < Command
    def help
      return "down [n]", "Change the eval context by moving down 1 (or n) call frames."
    end

    def command_regexp
      /^down(?:\s+(\d+))?$/
    end

    def execute(dbg, interface, md)
      n = (md[1] or "1").to_i
      ctxt = interface.debug_context
      frames = [ctxt]
      while ctxt != interface.eval_context
        ctxt = ctxt.sender
        frames.unshift ctxt
      end
      n.downto(0) { ctxt = frames.shift if frames.size > 0 }
      interface.eval_context = ctxt
      output = Output.info("Evaluation context now at:")
      output.set_columns(['%*s', '%|s', '%-*s'])
      output.set_line_marker
      loc = ctxt.location
      bt = Backtrace.new
      if ctxt == interface.debug_context
        output.set_color :green
      elsif loc =~ /kernel/
        output.set_color bt.kernel_color
      elsif loc =~ /\(eval\)/
        output.set_color bt.eval_color
      end
      output << [ctxt.describe, 'at', loc]
      output
    end
  end
end
