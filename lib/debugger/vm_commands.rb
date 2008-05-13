# Defines debugger commands that inspect and manipulate the Rubinius VM directly
class Debugger

  # Step in to the next VM instruction
  class StepInInstruction < Command
    def help
      return "s[tep]i [to] [n]", "Step to the next, (or nth next, or target IP) VM instruction, stepping into called methods."
    end

    def command_regexp
      /^s(?:tep)?\s*i(?:nst(?:ruction)?)?(?:\s+(?:(to)\s+)?(\d+))?$/
    end

    def execute(dbg, interface, md)
      step_type = md[1]
      n = md[2]
      steps = nil

      selector = {:step_type => :in, :step_by => :ip}
      if step_type
        selector[:target] = n.to_i
        output = Output.info("Stepping to IP[#{n}]")
      else
        n = selector[:steps] = n ? n.to_i : 1
        output = Output.info("Stepping #{n} instruction#{'s' unless n.to_i == 1}")
      end
      dbg.step(selector)

      # Instruct debugger to end session and resume debug thread
      interface.done!

      return output
    end
  end

  # Step next to the next VM instruction
  class StepNextInstruction < Command
    def help
      return "n[ext]i [to] [n]", "Step to the next, (or nth next, or target IP) VM instruction, without stepping into called methods."
    end

    def command_regexp
      /^n(?:ext)?\s*i(?:nst(?:ruction)?)?(?:\s+(?:(to)\s+)?(\d+))?$/
    end

    def execute(dbg, interface, md)
      step_type = md[1]
      n = md[2]
      steps = nil

      selector = {:step_type => :next, :step_by => :ip}
      if step_type
        selector[:target] = n.to_i
        output = Output.info("Stepping to IP[#{n}]")
      else
        n = selector[:steps] = n ? n.to_i : 1
        output = Output.info("Stepping #{n} instruction#{'s' unless n.to_i == 1}")
      end
      dbg.step(selector)

      # Instruct debugger to end session and resume debug thread
      interface.done!

      return output
    end
  end


  # Lists the VM bytecode
  # TODO: Should bytecode listing remember last window displayed and show next?
  class ListBytecode < Command
    @@re = Regexp.new('^d(?:ecode)?(?:\s+' + MODULE_METHOD_RE + ')?(?:\s+(\d+))?(?:(?:\s+|-)(\d+))?$')
    def help
      return "d[ecode] [<method>] [start [end]]", "Decode bytecode around breakpoint or between start/end."
    end

    def command_regexp
      @@re
    end

    def execute(dbg, interface, md)
      mod, mthd_type, mthd = md[1], md[2], md[3]
      first, last = md[4], md[5]

      if mthd
        cm = get_method(mod, mthd_type, mthd).compiled_method
        first = 0 unless first
      else
        # Decode current method
        cm = interface.eval_context.method
        first = interface.eval_context.ip - 10 unless first
      end
      file = cm.file.to_s
      lines = dbg.source_for(file)
      asm = dbg.asm_for(cm)

      first = first.to_i
      last = last.to_i if last
      last = first + 20 unless last
      first, last = last, first if first > last
      first = 0 if first < 0

      output = Output.info("Bytecode instructions [#{first}-#{last}] in compiled method #{cm.name}:")
      output.set_columns(["%04d:", "%-s ", "%-*s"])
      output << [nil, '...', nil] if first > 0
      line = 0
      asm.each do |inst|
        if inst.ip >= first and inst.ip <= last
          if inst.line != line
            line = inst.line
            src = lines[line-1] if lines
            output.set_color :green
            output << [nil, "# line #{line}:", src]
            output.set_color :clear
          end
          if !mthd and inst.ip == interface.eval_context.ip
            output.set_line_marker
            output.set_color :cyan
          end
          output << [inst.ip, inst.opcode, inst.args.map{|a| a.inspect}.join(', ')]
          output.set_color(:clear) if inst.ip == interface.eval_context.ip
        end
        break if inst.ip >= last
      end
      output << [nil, '...', nil] if last < asm[-1].ip

      return output
    end
  end


  # Shows the contents of the VM stack
  class ShowStack < Command
    def help
      return "v[m] s[tack]", "Display the current objects on the VM stack."
    end

    def command_regexp
      /^v(?:m)?\s*s(?:tack)?(?:\s+(\d+))?(?:\s+(\d+))?$/
    end

    def execute(dbg, interface, md)
      first, last = md[1], md[2]
      first = first.to_i if first
      last = last.to_i if last
      first = 0 unless first
      last = first+5 unless last
      first, last = last, first if first > last

      task = interface.debug_thread.task
      fp = interface.debug_context.fp
      top = interface.debug_context.sp

      output = Output.info("VM stack [#{first}-#{last}]:")
      output.set_columns([['Depth', "%2d:"], ['Class', '%-s'], ['Value', ' %-*s']])
      output << [nil, '...', nil] if first > 0
      first.upto(last) do |i|
        val = task.get_stack_value(i)
        sp = top - i
        output.set_color(:yellow) if sp <= fp
        if sp == top && sp == fp
          output.set_line_marker "fp|sp => "
        elsif sp == top
          output.set_line_marker "sp => "
        elsif sp == fp
          output.set_line_marker "fp => "
        end
        output << [i, val.class, val.inspect]
      end
      output << [nil, '...', nil] if last < task.stack_size-1

      return output
    end
  end
  
  
  # Shows info about the SendSites within the current method
  class ShowSendSites < Command
    @@re = Regexp.new('^v(?:m)?\s*s(?:end)?\s*s(?:ite(?:s)?)?(?:\s+' + METHOD_RE + ')?$')

    def help
      return "v[m] s[end] s[ites] [<selector>]", "Display send site info for each send site in the current method, or for the specified selector."
    end
    
    def command_regexp
      @@re
    end
    
    def execute(dbg, interface, md)
      selector = md[1]
      if selector
        source = Selector::ALL[selector.intern]
        if source
          send_sites = source.send_sites
          output = output(source, send_sites)
        else
          output = Output.error("No selector found with name '#{selector}'")
        end
      else
        source = interface.eval_context.method
        send_sites = source.send_sites
        if send_sites and send_sites.size > 0
          output = output(source, send_sites)
        else
          output = Output.none("There are no send sites in method #{source.name}")
        end
      end
      output
    end

    def output(source, send_sites)
      i = 1
      output = Output.info("SendSites for #{'selector ' if source.kind_of? Selector}#{source.name}:")
      output.set_columns(['%-3d.', ['Sender file', '%-*s'], ['Sender method', '%-*s'],
                          ['Message', '%-s'], ['Receiver class', '%-*s'], ['Method', '%-*s'],
                          ['Module', '%-*s'], ['Hits', '%d'], ['Misses', '%d']])
      send_sites.each do |ss|
        runnable_name = nil
        case runnable = ss.data(2)
        when CompiledMethod
          runnable_name = runnable.name
        when RuntimePrimitive
          runnable_name = runnable.at RuntimePrimitive::PrimitiveIndex
          runnable_name = Rubinius::Primitives[runnable_name] if runnable_name.kind_of? Fixnum
        when NilClass
          # Do nothing - SendSite data2 not set
        else
          STDERR.puts "Unrecognized runnable type: #{runnable.class}"
        end
        output << [i, ss.sender.file, ss.sender.name, ss.name, ss.data(1), 
                   runnable_name, ss.data(3), ss.hits, ss.misses]
        i += 1
      end
      output
    end
  end
end

