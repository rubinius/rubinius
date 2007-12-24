# Defines debugger commands that inspect and manipulate the Rubinius VM directly
class Debugger

  # Steps to the next VM instruction
  class StepNextInstruction < Command
    def help
      return "n[ext] i[nstruction] [+n]", "Step to the next, (or nth next) VM instruction without stepping into called methods"
    end

    def command_regexp
      /^n(?:ext)?\s*i(?:nst(?:ruction)?)?(?:\s+(\+)?(\d+))?$/
    end

    def execute(dbg, md)
      # Determine how many instructions to step ahead
      step_type = md[1]
      n = md[2]
      steps = nil
      target_ip = nil

      # Find IP of instruction to step to
      ctxt = dbg.debug_context
      cm = ctxt.method
      bc = cm.decode

      if n.nil?
        steps = 1
      elsif step_type.nil?
        target_ip = n.to_i
      else
        steps = n.to_i
        steps = 1 if steps < 1
      end

      # Locate the IP at n steps past the current IP
      bp_ip = nil
      bc.each do |op|
        if steps
          steps -= 1 if op.ip > ctxt.ip
          if steps == 0
            bp_ip = op.ip
            break
          end
        else
          # Ensure target_ip is valid
          if op.ip >= target_ip
            bp_ip = op.ip
            break
          end
        end
      end

      if bp_ip
        # Set new breakpoint
        dbg.set_breakpoint cm, bp_ip
        ctxt.reload_method
        output = "Stepping to IP:#{bp_ip}"
      else
        # Stepping past end of method, so set breakpoint at caller
        ctxt = ctxt.sender
        dbg.set_breakpoint ctxt.method, ctxt.ip
        ctxt.reload_method
        output = "Stepping out to caller (#{ctxt.method.name}) [IP:#{ctxt.ip}]"
      end

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
      return "d[ecode] [start [end]]", "Decode bytecode around breakpoint or between start/end"
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
      return "v[m] s[tack]", "Display the current objects on the VM stack"
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
end