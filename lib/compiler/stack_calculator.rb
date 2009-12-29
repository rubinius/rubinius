module Rubinius
  class CompilerNG
    class StackDepthCalculator
      def initialize(stream, lines)
        @iseq = stream
        @lines = lines
        @show_stack = false
      end

      def run
        @stack_at = []
        @last_start = []
        @max_stack = 0
        run_from(0,0)
        @max_stack
      end

      def ip_to_line(ip)
        total = @lines.size - 2
        i = 0

        while i < total
          if ip >= @lines[i] and ip <= @lines[i+2]
            return @lines[i+1]
          end

          i += 2
        end
      end

      def run_from(start_ip, current_stack)
        # if we've already started processing from this ip with at least
        # this much stack, no need to do it again.
        if sub_stack = @last_start[start_ip]
          # puts "stack_compare of #{start_ip}: #{sub_stack} >= #{current_stack}"
          return if sub_stack >= current_stack
        end

        # Remember how much stack we had when we entered this section
        @last_start[start_ip] = current_stack

        ip = start_ip
        total = @iseq.size

        while ip < total
          if last_time = @stack_at[ip]
            if last_time != current_stack
              line = ip_to_line ip
              raise "unbalanced stack at line #{line} (#{ip}): #{last_time} != #{current_stack}"
            end
          end

          @stack_at[ip] = current_stack

          opcode = Rubinius::InstructionSet.opcodes[@iseq[ip]]

          # if @show_stack
          #   puts "%3d: %21s %3d %3d" % [ip, opcode.opcode.to_s, current_stack, @max_stack]
          # end

          if opcode.variable_stack?
            current_stack += opcode.stack_difference - @iseq[ip + opcode.position]
          else
            current_stack += opcode.stack_difference
          end

          raise "stack underflow at line #{ip_to_line ip} (#{ip})" if current_stack < 0

          @max_stack = current_stack if current_stack > @max_stack

          case opcode.opcode
          when :setup_unwind, :goto_if_false, :goto_if_true, :goto_if_defined
            dest = @iseq[ip + 1]
            # Only process forward jumps
            # TODO might be neat to add the ability to make sure this loop
            # left the stack exactly as it found it.
            if dest > ip
              run_from(dest, current_stack)
            end
          when :goto
            dest = @iseq[ip + 1]

            # Only process forward jumps
            if dest > ip
              run_from(dest, current_stack)
            end

            return
          when :ret, :raise_exc, :raise_break, :raise_return, :ensure_return,
               :reraise
            return
          end
          ip += opcode.size
        end

        raise "Control flowed off end of InstructionSequence"
      end
    end
  end
end
