# Defines debugger commands that inspect and manipulate the Rubinius VM directly
class Debugger

  # Step in to the next VM instruction
  class StepInInstruction < Command
    def help
      return "s[tep]i [+n|line]", "Step to the next, (or nth next) VM instruction, stepping into called methods."
    end

    def command_regexp
      /^s(?:tep)?i(?:\s+(\+)?(\d+))?$/
    end

    def execute(dbg, md)
      step_type = md[1]
      n = md[2]
      steps = nil
      target_line = nil

      selector = {:step_type => :in, :step_by => :ip}
      if step_type
        selector[:target] = n
        output = "Stepping to IP[#{n}]"
      else
        n = selector[:steps] = n ? n.to_i : 1
        output = "Stepping #{n} instruction#{'s' unless n.to_i == 1}"
      end
      dbg.step(selector)

      # Instruct debugger to end session and resume debug thread
      dbg.done!

      return output
    end
  end

  # Step next to the next VM instruction
  class StepNextInstruction < Command
    def help
      return "n[ext]i [+n|line]", "Step to the next, (or nth next) VM instruction, without stepping into called methods."
    end

    def command_regexp
      /^n(?:ext)?\s*i(?:nst(?:ruction)?)?(?:\s+(\+)?(\d+))?$/
    end

    def execute(dbg, md)
      step_type = md[1]
      n = md[2]
      steps = nil
      target_line = nil

      selector = {:step_type => :next, :step_by => :ip}
      if step_type
        selector[:target] = n
        output = "Stepping to IP[#{n}]"
      else
        n = selector[:steps] = n ? n.to_i : 1
        output = "Stepping #{n} instruction#{'s' unless n.to_i == 1}"
      end
      dbg.step(selector)

      # Instruct debugger to end session and resume debug thread
      dbg.done!

      return output
    end
  end


  # Lists the VM bytecode
  # TODO: Add ability to list bytecode for a specified method
  # TODO: Should bytecode listing remember last window displayed and show next?
  class ListBytecode < Command
    def help
      return "d[ecode] [start [end]]", "Decode bytecode around breakpoint or between start/end."
    end

    def command_regexp
      /^d(?:ecode)?(?:\s+(\d+))?(?:\s+(\d+))?$/
    end

    def execute(dbg, md)
      first,last = md[1], md[2]
      first = first.to_i if first
      last = last.to_i if last

      asm = dbg.debug_context.method.decode
      first = dbg.debug_context.ip - 10 unless first
      last = first + 20 unless last
      first, last = last, first if first > last
      first = 0 if first < 0

      output = Output.new
      output << "Bytecode instructions [#{first}-#{last}] in compiled method #{dbg.debug_context.method.name}:"
      output.set_columns(["%04d:", "%-s ", "%s"])
      output << [nil, '...', nil] if first > 0
      line = 0
      asm.each do |inst|
        if inst.ip >= first and inst.ip <= last
          if inst.line != line
            output << [nil, "# line #{inst.line}", nil]
            line = inst.line
          end 
          if inst.ip == dbg.debug_context.ip
            output.set_line_marker
            output.set_color :cyan
          end
          output << [inst.ip, inst.opcode, inst.args.map{|a| a.inspect}.join(', ')]
          output.set_color(:clear) if inst.ip == dbg.debug_context.ip
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

    def execute(dbg, md)
      first, last = md[1], md[2]
      first = first.to_i if first
      last = last.to_i if last
      first = 0 unless first
      last = first+5 unless last
      first, last = last, first if first > last

      task = dbg.debug_thread.task
      fp = dbg.debug_context.fp
      top = dbg.debug_context.sp

      output = Output.new
      output << "VM stack [#{first}-#{last}]:"
      output.set_columns(['%2d:', '%-s', ' %s'])
      output << [nil, '...', nil] if first > 0
      first.upto(last) do |i|
        val = task.get_stack_value(i)
        sp = top - i
        output.set_color(:yellow) if sp <= fp
        if sp == top && sp == fp
          output.set_line_marker "fp|sp =>"
        elsif sp == top
          output.set_line_marker "sp =>"
        elsif sp == fp
          output.set_line_marker "fp =>"
        end
        output << [i, val.class, val.inspect]
      end
      output << [nil, '...', nil] if last < task.stack_size-1

      return output
    end
  end
  
  
  # Shows info about the SendSites within the current method
  class ShowSendSites < Command
    def help
      return "v[m] s[end] s[ites]", "Display send site info for each send site in the current method."
    end
    
    def command_regexp
      /^v(?:m)?\s*s(?:end)?\s*s(?:ite(?:s)?)?$/
    end
    
    def execute(dbg, md)
      cm = dbg.debug_context.method
      literals = cm.literals

      i = 0
      if literals
        output = Output.new
        output << "SendSites for #{cm.name}:"
        output.set_columns(['%-3d.', '%-s', '%-s', '%-s', '%-s', '%d', '%d'])
        literals.each do |lit|
          if lit.kind_of? SendSite
            runnable_name = nil
            case runnable = lit.data(2)
            when CompiledMethod
              runnable_name = runnable.name
            when RuntimePrimitive
              runnable_name = runnable.at RuntimePrimitive::PrimitiveIndex
              runnable_name = Rubinius::Primitives[runnable_name] if runnable_name.kind_of? Fixnum
            when NilClass
              # Do nothing - SendSite data2 not set
            else
              puts "Unrecognized runnable type: #{runnable.class}"
            end
            output << [i+1, lit.name, lit.data(1), runnable_name, lit.data(3), lit.hits, lit.misses] if lit
            i += 1
          end
        end
      end
      output = "There are no send sites in method #{cm.name}" if i == 0
      output
    end
  end
end