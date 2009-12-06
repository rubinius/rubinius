module Rubinius
  # Defines debugger commands that inspect and manipulate the Rubinius VM directly
  class Debugger

    # Step in to the next VM instruction
    class StepInInstruction < Command
      command :step_in_instruction,
        :regexp => /^s(?:tep)?\s*i(?:nst(?:ruction)?)?(?:\s+(?:(to)\s+)?(\d+))?$/,
        :syntax => "s[tep]i [to] [n]",
        :description => <<-END
          Step to the next, (or nth next, or target IP) VM instruction, stepping
          into called methods.
        END

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
      command :step_next_instruction,
        :regexp => /^n(?:ext)?\s*i(?:nst(?:ruction)?)?(?:\s+(?:(to)\s+)?(\d+))?$/,
        :syntax => "n[ext]i [to] [n]",
        :description => <<-END
          Step to the next, (or nth next, or target IP) VM instruction, without
          stepping into called methods.
        END

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
      command :decode,
        :regexp => Regexp.new('^d(?:ecode)?(?:\s+' + MODULE_METHOD_RE +
          ')?(?:\s+(\d+))?(?:(?:\s+|-)(\d+))?$'),
        :syntax => "d[ecode] [<method>] [start [end]]",
        :description => "Decode bytecode around breakpoint or between start/end."

      setting :window_size, 20,
        :description => <<-END
          Sets the 'window' size, i.e. the number of instructions to be displayed
          around the current IP when no IP range is specified.
        END
      setting :auto_advance, false, "Determines whether the listing advances if the command is repeated"

      def execute(dbg, interface, md)
        mod, mthd_type, mthd = md[1], md[2], md[3]
        first, last = md[4], md[5]

        if mthd
          cm = get_method(mod, mthd_type, mthd).compiled_method
          first = 0 unless first
        else
          # Decode current method
          cm = interface.eval_context.method
          first = interface.eval_context.ip - @window_size/2 unless first
        end
        file = cm.file.to_s
        lines = dbg.source_for(file)
        asm = dbg.asm_for(cm)

        first = first.to_i
        first = 0 if first < 0
        if last
          last = last.to_i
        else
          last = first + @window_size
        end
        first, last = last, first if first > last

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
      command :stack,
        :regexp => /^v(?:m)?\s*s(?:tack)?(?:\s+(\d+))?(?:\s+(\d+))?$/,
        :syntax => "v[m] s[tack] [start [end]]",
        :description => "Display the current objects on the VM stack."

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
      command :send_sites,
        :regexp => Regexp.new('^v(?:m)?\s*s(?:end)?\s*s(?:ite(?:s)?)?(?:\s+' + METHOD_RE + ')?$'),
        :syntax => "v[m] s[end] s[ites] [<selector>]",
        :description => "Display send site info for each send site in the current method, or for the specified selector."

      def execute(dbg, interface, md)
        selector = md[1]
        if selector
          source = Rubinius::Selector::ALL[selector.intern]
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
        output = Output.info("SendSites for #{'selector ' if source.kind_of? Rubinius::Selector}#{source.name}:")
        if source.kind_of? Rubinius::Selector
          output.set_columns(['%-3d.', ['Sender file', '%-*s'], ['Sender method', '%-*s'],
                            ['Message', '%-s'], ['Receiver class', '%-*s'], ['Method', '%-*s'],
                            ['Module', '%-*s'], ['Hits', '%d'], ['Misses', '%d']])
        else
          output = Output.info("SendSites for #{'selector ' if source.kind_of? Rubinius::Selector}#{source.name}:")
          output.set_columns(['%-3d.', ['Message', '%-s'], ['Receiver class', '%-*s'],
                             ['Method', '%-*s'], ['Module', '%-*s'], ['Hits', '%d'],
                             ['Misses', '%d']])
        end
        send_sites.each_with_index do |ss,i|
          if source.kind_of? Rubinius::Selector
            sender_file = ss.sender.file if ss.sender
            sender_name = ss.sender.name if ss.sender
            output << [i+1, sender_file, sender_name, ss.name,
                       ss.recv_class, ss.method, ss.module, ss.hits, ss.misses]
          else
            output << [i+1, ss.name, ss.recv_class, ss.method, ss.module, ss.hits, ss.misses]
          end
        end
        output
      end
    end
  end
end

