module Rubinius
  class Sprinter
    class << self
      def get(format)
        @cache ||= {}

        # An alternative to the below would be to have the Builder
        # ignore the taint of the format, and instead have sprintf check
        # the format upon invocation, and taint the result there.
        if format.tainted?
          new(format)
        else
          @cache[format] ||
            (@cache[format] = new(format))
        end
      end
    end

    def initialize(format)
      Builder.new(self, format).build
    end

    class Builder
      def initialize(code, format)
        @code, @format = code, format

        @append_parts = 0

        # Change next line to 'true' for debug output. Can't really use
        # $DEBUG, because of its special meaning to sprintf.
        if @verbose = false # (format == "%s - %s") # , %d, %02d, %s")
          @@seen ||= {}
          if @@seen[format]
            @verbose = false
          else
            @@seen[format] = true
          end
        end

        @g = Rubinius::Generator.new
        @g.name = :call
        @g.file = :sprintf
        @g.set_line 1
      end

      def build
        self.parse

        @g.required_args = @arg_count

        @g.total_args = @arg_count + 1
        # We won't use it, but we accept a splat; our semantics require
        # that we ignore any excess arguments provided.
        @g.splat_index = @arg_count

        @g.local_count = @arg_count + 1
        if @index_mode == :absolute
          @g.local_names = (0...@arg_count).map {|i| :"#{i + 1}$" } + [:splat]
        else
          @g.local_names = (0...@arg_count).map {|i| :"arg#{i}" } + [:splat]
        end

        @g.string_build @append_parts
        @g.ret
        @g.close

        begin
          @g.encode
        rescue Exception
          if $DEBUG
            puts "Error compiling printf pattern: #{@format}"
            puts "Literals: #{@g.literals.inspect}"
            decoder = Rubinius::InstructionDecoder.new(@g.iseq)
            ip = 0
            decoder.decode(false).each do |i|
              if i.size == 1
                puts "#{ip}:\t#{i[0].opcode}"
              else
                puts "#{ip}:\t#{i[0].opcode}, #{i[1..-1].inspect}"
              end
              ip += i.size
            end
          end
          raise
        end

        cm = @g.package Rubinius::CompiledMethod

        # Careful with this: CM::Instruction#to_s uses String#%
        if @verbose
          puts
          puts @format.inspect
          puts cm.decode
          puts
        end

        ss = Rubinius::StaticScope.new Object
        Rubinius.attach_method @g.name, cm, ss, @code
      end

      def meta_op_minus
        @op_minus ||= @g.find_literal(:-)
        @g.meta_send_op_minus @op_minus
      end

      def invert
        @g.meta_push_0
        @g.swap
        meta_op_minus
      end

      def is_negative
        @g.meta_push_0
        @g.meta_send_op_lt @g.find_literal(:<)
      end

      def justify(direction, may_be_negative=true)
        if may_be_negative && direction != :ljust
          width_done = @g.new_label

          @g.dup
          is_negative
          if_false do
            @g.send direction, 1
            @g.goto width_done
          end

          invert
          @g.send :ljust, 1

          width_done.set!
        else
          @g.send direction, 1
        end
      end

      RADIX = { 'd' => 10, 'i' => 10, 'u' => 10, 'x' => 16, 'o' => 8, 'b' => 2 }
      PREFIX = { 'o' => '0', 'x' => '0x', 'X' => '0X', 'b' => '0b', 'B' => '0B' }

      def next_index(specified=nil)
        if specified
          specified = specified.to_i
          raise ArgumentError, "invalid positional index" if specified == 0

          if @index_mode == :relative
            raise ArgumentError, "unnumbered mixed with numbered"
          end

          @index_mode = :absolute
          @arg_count = specified if specified > @arg_count
          specified - 1
        else
          if @index_mode == :absolute
            raise ArgumentError, "unnumbered mixed with numbered"
          end
          @index_mode = :relative
          (@arg_count += 1) - 1
        end
      end

      RE = /
        ([^%]+|%(?:[\n\0]|\z)) # 1
        |
        %
        ( # 2
          ([0# +-]*) # 3
          (?:([0-9]+)\$)? # 4
          ([0# +-]*) # 5
          (?:
            (\*(?:([0-9]+)\$)?|([1-9][0-9]*))? # 6 7 8
            (?:\.(\*(?:([0-9]+)\$)?|([1-9][0-9]*))?)? # 9 10 11
          )
          (?:([0-9]+)\$)? # 12
          ([BbcdEefGgiopsuXx]) # 13
        )
        |
        (%)(?:%|[-+0-9# *.$]+\$[0-9.]*\z) # 14
        |
        (%) # 15
      /x

      def append_literal(str)
        @g.push_unique_literal str
        append_str false
      end

      def append_str(taint)
        @append_parts += 1
        taint = false

        return

        if @has_content
          if taint
            @g.swap
            @g.move_down 2
          end

          @g.swap
          @g.string_append
        else
          @g.string_dup
          @has_content = true

          if @pre_tainted
            @g.send :taint, 0
          end
        end

        if taint
          @g.swap
          tainting_done = @g.new_label
          @g.send :tainted?, 0
          if_true do
            @g.send :taint, 0
          end
        end
      end

      class Atom
        def initialize(b, g, format_code, flags)
          @b, @g = b, g
          @format_code, @flags = format_code, flags

          @f_alt = flags.index(?#)
          @f_zero = flags.index(?0)
          @f_plus = flags.index(?+)
          @f_ljust = flags.index(?-)
          @f_space = flags.index(?\ )

          @just_dir = @f_ljust ? :ljust : :rjust

          @prefix = PREFIX[@format_code] if @f_alt

          @full_leader_size = @prefix ? @prefix.size : 0
          @full_leader_size += 1 if @f_plus || @f_space

        end

        def prepend_prefix
          if @prefix
            @g.push_literal @prefix
            @g.string_dup
            @g.string_append
          end
        end

        def set_value(ref)
          @field_index = @b.next_index(ref)
        end

        def set_width(full, ref, static)
          @width_static = static && static.to_i
          if full && !static
            @width_index = @b.next_index(ref)
          end

          @has_width = @width_static || @width_index
        end

        def set_precision(full, ref, static)
          @prec_static = static && static.to_i
          if full && !static
            @prec_index = @b.next_index(ref)
          end

          if @format_code == 'g' && @f_alt && !full
            @prec_static = 4
          end

          @has_precision = @prec_static || @prec_index
        end

        def push_value
          @g.push_local @field_index
        end

        def push_width(adjust=true)
          yield if block_given?
          if @width_static
            raise ArgumentError, "width too big" unless @width_static.class == Fixnum
            if adjust && @full_leader_size > 0
              @g.push(@width_static - @full_leader_size)
            else
              @g.push @width_static
            end

            if block_given?
              @g.swap
              @b.if_true do
                @g.meta_push_1
                @b.meta_op_minus
              end
            end

          elsif @width_index
            @g.push_local @width_index

            @b.force_type :Fixnum, :Integer do
              # If we had to do a conversion, we check we ended up
              # with a Fixnum
              @g.dup
              @b.push_Fixnum
              @g.swap
              @g.kind_of

              @b.if_false do
                @b.raise_ArgumentError "width too big"
              end
            end

            n = adjust ? @full_leader_size : 0
            if block_given?
              adjusted = @g.new_label

              @g.swap
              @b.if_true do
                @g.push n + 1
                @b.meta_op_minus
                if n > 0
                  @g.goto adjusted
                end
              end

              if n > 0
                @g.push n
                @b.meta_op_minus
                adjusted.set!
              end

            elsif n > 0
              @g.push n
              @b.meta_op_minus
            end

          else
            raise "push without a width"

          end
        end

        def push_precision
          yield if block_given?
          if @prec_static
            raise ArgumentError, "precision too big" unless @prec_static.class == Fixnum
            @g.push @prec_static

          elsif @prec_index
            @g.push_local @prec_index

            @b.force_type :Fixnum, :Integer do
              # If we had to do a conversion, we check we ended up
              # with a Fixnum
              @g.dup
              @b.push_Fixnum
              @g.swap
              @g.kind_of

              @b.if_false do
                @b.raise_ArgumentError "precision too big"
              end
            end

          else
            raise "push without a precision"

          end

          if block_given?
            @g.swap
            @b.if_true do
              @g.meta_push_1
              @b.meta_op_minus
            end
          end
        end

        def push_format_string
          float_format_code = @format_code
          float_format_code = 'f' if @format_code == 'g' && @f_alt

          leader = "%#{@flags}"
          if !@width_index && !@prec_index
            leader << @width_static.to_s if @width_static
            leader << ".#{@prec_static}" if @prec_static
            @g.push_literal "#{leader}#{float_format_code}"
          else
            format_parts = 1
            if @prec_static
              @g.push_literal ".#{@prec_static}#{float_format_code}"
            else
              @g.push_literal(float_format_code)
              if @prec_index
                push_precision
                @g.send :to_s, 0
                format_parts += 1

                if @width_index
                  @g.push_literal "."
                  format_parts += 1
                end
              end
            end

            if @width_static
              leader << @width_static.to_s
            elsif @width_index
              push_width
              @g.send :to_s, 0
              format_parts += 1
            end
            leader << "." if @prec_index && !@width_index

            @g.push_literal leader
            @g.string_dup
            format_parts.times do
              @g.string_append
            end
          end
        end

        def positive_sign
          if @f_plus
            '+'
          elsif @f_space
            ' '
          else
            ''
          end
        end

        def justify_width(adjust=true)
          if @has_width
            push_width adjust
            @b.justify @just_dir, @width_static.nil?
          end
        end

        def zero_pad?
          @has_precision || (@has_width && @f_zero)
        end

        def zero_pad(pad="0", &readjust)
          if @has_precision
            push_precision &readjust
            @g.push_literal pad
            @g.send :rjust, 2
          elsif @has_width && @f_zero
            push_width true, &readjust
            @g.push_literal pad
            @g.send :rjust, 2
          end
        end

        def width?
          @has_width
        end

        def precision?
          @has_precision
        end
      end

      def push_Kernel
        @lit_Kernel ||= @g.add_literal(:Kernel)
        @slot_Kernel ||= @g.add_literal(nil)

        @g.push_const_fast @lit_Kernel, @slot_Kernel
      end

      def push_Fixnum
        @lit_Fixnum ||= @g.add_literal(:Fixnum)
        @slot_Fixnum ||= @g.add_literal(nil)

        @g.push_const_fast @lit_Fixnum, @slot_Fixnum
      end

      def raise_ArgumentError(msg)
        @lit_ArgumentError ||= @g.add_literal(:ArgumentError)
        @slot_ArgumentError ||= @g.add_literal(nil)

        @lit_new ||= @g.add_literal(:new)

        @g.push_const_fast @lit_ArgumentError, @slot_ArgumentError
        @g.push_unique_literal msg
        @g.send_stack @lit_new, 1
        @g.raise_exc
      end

      def force_type(klass, method=klass)
        @g.dup
        @g.push_const klass
        @g.swap
        @g.kind_of
        if_false do
          @g.push_self
          @g.swap
          @g.send method, 1, true

          yield if block_given?
        end
      end

      def if_true
        l = @g.new_label
        @g.gif l
        yield
        l.set!
      end

      def if_false
        l = @g.new_label
        @g.git l
        yield
        l.set!
      end

      def parse
        @arg_count = 0
        @index_mode = nil

        @pre_tainted = false  # @format.tainted?

        bignum_width = bignum_precision = nil

        pos = 0
        while match = RE.match_start(@format, pos)
          pos = match.end(0)

          _,
          plain_string,
          whole_format,
          flags_a,
          field_ref_a,
          flags_b,
          width_full, width_ref, width_static,
          prec_full, prec_ref, prec_static,
          field_ref_b,
          format_code,
          literal_char,
          invalid_format = *match

          if plain_string
            append_literal plain_string
          elsif literal_char
            append_literal literal_char
          elsif invalid_format || (field_ref_a && field_ref_b)
            raise ArgumentError, "malformed format string"
          else
            field_ref = field_ref_a || field_ref_b
            flags = "#{flags_a}#{flags_b}"

            alt = flags.index(?#)
            zero = flags.index(?0)
            plus = flags.index(?+)
            ljust = flags.index(?-)
            space = flags.index(?\ )


            atom = Atom.new(self, @g, format_code, flags)
            atom.set_width width_full, width_ref, width_static
            atom.set_precision prec_full, prec_ref, prec_static
            atom.set_value field_ref

            case format_code
            when 's', 'p', 'c'
              atom.push_value

              case format_code
              when 's'
                force_type :String

              when 'c'
                force_type :Fixnum, :Integer

                chr_range_ok = @g.new_label

                @g.dup
                @g.push 256
                @g.meta_send_op_lt @g.find_literal(:<)
                if_true do
                  @g.dup
                  @g.meta_push_neg_1
                  @g.meta_send_op_gt @g.find_literal(:>)
                  @g.git chr_range_ok
                end

                @g.push 256
                @g.send :%, 1

                chr_range_ok.set!
                @g.send :chr, 0
              when 'p'
                @g.send :inspect, 0
              end

              atom.justify_width

              if atom.precision?
                @g.meta_push_0
                atom.push_precision
                @g.send :[], 2
              end

              append_str !@pre_tainted

            when 'e', 'E', 'f', 'g', 'G'

              atom.push_value
              force_type :Float

              format_done = @g.new_label

              @g.dup
              @g.send :finite?, 0

              if_true do
                atom.push_format_string
                @g.send :to_s_formatted, 1, true

                @g.goto format_done
              end

              formatted_non_finite = @g.new_label

              @g.dup
              @g.send :nan?, 0

              if_false do
                is_negative

                if_false do
                  @g.push_literal "#{atom.positive_sign}Inf"
                  @g.goto formatted_non_finite
                end
                @g.push_literal '-Inf'
                @g.goto formatted_non_finite
              end

              @g.pop
              @g.push_literal 'NaN'

              formatted_non_finite.set!
              atom.justify_width false

              format_done.set!

              append_str false

            when 'd', 'i', 'u', 'B', 'b', 'o', 'X', 'x'
              radix = RADIX[format_code.downcase]

              atom.push_value

              # Bignum is obviously also perfectly acceptable. But we
              # just address the most common case by avoiding the call
              # if we've been given a Fixnum. The call is enough
              # overhead to bother, but not something to panic about.
              force_type :Fixnum, :Integer

              if plus || space || (zero && radix == 10 && format_code != 'u')
                @g.dup

                # stash away whether it's negative
                is_negative
                @g.dup
                @g.move_down 2

                if_true do
                  # but treat it as positive for now
                  invert
                end

                if radix == 10
                  @g.send :to_s, 0
                else
                  @g.push radix
                  @g.send :to_s, 1
                end
              elsif radix == 10 && format_code != 'u'
                @g.send :to_s, 0
              else
                have_formatted = @g.new_label

                @g.dup
                is_negative

                if_false do
                  if radix == 10
                    @g.send :to_s, 0
                  else
                    @g.push radix
                    @g.send :to_s, 1
                  end
                  @g.goto have_formatted
                end

                if format_code == 'u'
                  # Now we need to find how many bits we need to
                  # represent the number, starting with a native int,
                  # then incrementing by 32 each round.

                  more_bits_loop = @g.new_label
                  got_enough_bits = @g.new_label

                  # Push a positive version of the number ($N)
                  @g.dup
                  invert

                  # Push the baseline ($B), starting from a native int:
                  # 2**32 or 2**64, as appropriate
                  @g.meta_push_1
                  l_native = @g.find_literal(2.size * 8)
                  @g.push_literal_at l_native
                  @g.send :<<, 1

                  # Switch to $N
                  @g.swap
                  # For the first time, because it's what we've used
                  # above, we'll shift it by our native int size
                  @g.push_literal_at l_native

                  more_bits_loop.set!
                  # Throw out the bits from $N that $B can offset
                  @g.send :>>, 1

                  # Check whether $N == 0
                  @g.dup
                  @g.meta_push_0
                  @g.meta_send_op_equal @g.find_literal(:==)
                  @g.git got_enough_bits

                  # Switch to $B
                  @g.swap
                  l_32 = @g.find_literal(32)
                  @g.push_literal_at l_32
                  # Add 32 bits
                  @g.send :<<, 1
                  # Switch to $N
                  @g.swap
                  # We'll throw out 32 bits this time
                  @g.push_literal_at l_32
                  @g.goto more_bits_loop

                  got_enough_bits.set!
                  # Pop the spare copy of $N, which is 0
                  @g.pop


                  # Now we're left with $B; we can now use it, by adding
                  # it to the (negative) number still on the stack from
                  # earlier.

                  # $B is a Bignum; no point using meta_send_op_plus.
                  @g.send :+, 1
                  @g.send :to_s, 0

                  padding = "."

                else
                  # (num + radix ** num.to_s(radix).size).to_s(radix)
                  @g.push radix
                  @g.dup_many 2
                  @g.send :to_s, 1
                  @g.send :size, 0
                  @g.send :**, 1
                  @g.meta_send_op_plus @g.find_literal(:+)
                  @g.push radix
                  @g.send :to_s, 1

                  padding = (radix - 1).to_s(radix)
                end

                if atom.zero_pad?
                  atom.zero_pad padding

                elsif !atom.precision? && !zero
                  @g.push_literal ".."
                  @g.string_dup
                  @g.string_append
                end

                have_formatted.set!
              end

              # 'B' also returns an uppercase string, but there, the
              # only alpha character is in the prefix -- and that's
              # already uppercase
              if format_code == 'X'
                @g.send :upcase, 0
              end

              if !(plus || space) && (zero && radix == 10 && format_code != 'u')
                atom.zero_pad do
                  # If it decides to do any padding, zero_pad will yield
                  # before it modifies the stack, and we must ensure the
                  # top of the stack is a boolean indicating whether to
                  # subtract one from the requested width (for a minus
                  # sign to be prepended below), followed by the string-
                  # in-progress.

                  @g.swap
                  @g.dup
                  @g.move_down 2
                end
              else
                atom.zero_pad
              end

              atom.prepend_prefix

              if plus || space
                append_sign = @g.new_label

                @g.swap
                if_true do
                  @g.push_literal '-'

                  @g.goto append_sign
                end

                @g.push_literal atom.positive_sign

                append_sign.set!
                @g.string_dup
                @g.string_append

              elsif zero && radix == 10 && format_code != 'u'

                @g.swap
                if_true do
                  @g.push_literal '-'

                  @g.string_dup
                  @g.string_append
                end

              end


              if atom.precision? || !zero
                atom.justify_width false
              end

              append_str false

            else
              raise ArgumentError, "bad format character: #{format_code}"
            end
          end
        end

        if @index_mode != :absolute
          no_exception = @g.new_label

          # If we've used relative arguments, and $DEBUG is true, we
          # throw an exception if passed more arguments than we need.

          # Check this first; it's much faster, and generally false
          @g.passed_arg @arg_count
          @g.gif no_exception

          Rubinius::AST::GlobalVariableAccess.new(1, :$DEBUG).bytecode(@g)
          @g.gif no_exception

          raise_ArgumentError "too many arguments for format string"

          no_exception.set!
        end
      end
    end
  end
end
