# -*- encoding: us-ascii -*-

module Rubinius
  class Sprinter

    total = Rubinius::Config['printf.cache']

    case total
    when Fixnum
      if total == 0
        @cache = nil
      else
        @cache = Rubinius::LRUCache.new(total)
      end
    when false
      @cache = nil
    else
      @cache = Rubinius::LRUCache.new(50)
    end

    def self.cache
      @cache
    end

    def self.get(format)
      if ec = @cache
        if sprinter = ec.retrieve(format)
          return sprinter
        end
      end

      sprinter = new(format)

      ec.set(format, sprinter) if ec

      sprinter
    end

    def initialize(format)
      Rubinius::Type.object_singleton_class(self).dynamic_method :call do |g|
        Builder.new(self, format, g).build
      end
    end

    def debug_print
      printer = Compiler::MethodPrinter.new
      printer.input(method(:call).executable)
      printer.bytecode = true
      printer.run
    end

    def zero_two_expand_integer(int)
      int = Integer(int) unless int.kind_of? Fixnum

      return int.to_s if int < 0 or int >= 10

      "0#{int}"
    end

    def zero_expand_integer(int, width)
      int = Integer(int) unless int.kind_of? Fixnum

      str = int.to_s
      return str if width <= str.size
      diff = width - str.size

      if int < 0
        case diff
        when 1
          "-0#{-int}"
        when 2
          "-00#{-int}"
        when 3
          "-000#{-int}"
        else
          "-#{'0' * diff}#{-int}"
        end
      else
        case diff
        when 1
          "0#{str}"
        when 2
          "00#{str}"
        when 3
          "000#{str}"
        when 4
          "0000#{str}"
        else
          ("0" * diff) << str
        end
      end
    end

    def zero_expand_leader(int, width)
      if int < 0
        zero_expand_integer(int, width)
      else
        zero_expand_integer(int, width-1)
      end
    end

    def space_expand_integer(int, width)
      int = Integer(int) unless int.kind_of? Fixnum

      str = int.to_s

      width = -width if width < 0
      return str if width <= str.size
      diff = width - str.size

      case diff
      when 1
        " #{str}"
      when 2
        "  #{str}"
      when 3
        "   #{str}"
      when 4
        "    #{str}"
      else
        (" " * diff) << str
      end
    end

    def space_expand_leader(int, width)
      if int < 0
        space_expand_integer(int, width)
      else
        space_expand_integer(int, width-1)
      end
    end

    def space_expand_integer_left(int, width)
      int = Integer(int) unless int.kind_of? Fixnum

      str = int.to_s
      return str if width <= str.size
      diff = width - str.size

      case diff
      when 1
        "#{str} "
      when 2
        "#{str}  "
      when 3
        "#{str}   "
      when 4
        "#{str}    "
      else
        str + (" " * diff)
      end
    end

    def space_expand_leader_left(int, width)
      if int < 0
        space_expand_integer_left(int, width)
      else
        space_expand_integer_left(int, width-1)
      end
    end

    def as_int(int)
      if int.kind_of? Integer
        return int
      else
        Integer(int)
      end
    end

    def compute_space(int)
      if int >= 0
        " "
      else
        ""
      end
    end

    def compute_plus(int)
      if int >= 0
        "+"
      else
        ""
      end
    end

    def digit_expand_precision(int, precision)
      str = int.to_s

      diff = Integer(precision) - str.size

      if int < 0
        # account for the - sign infront
        diff += 1

        return str if diff <= 0

        case diff
        when 1
          "-0#{-int}"
        when 2
          "-00#{-int}"
        when 3
          "-000#{-int}"
        else
          "-#{'0' * diff}#{-int}"
        end
      else
        return str if diff <= 0
        case diff
        when 1
          "0#{str}"
        when 2
          "00#{str}"
        when 3
          "000#{str}"
        when 4
          "0000#{str}"
        else
          ("0" * diff) << str
        end
      end
    end

    def space_expand_left(str, width)
      width = Integer(width)

      sz = str.size

      width = -width if width < 0
      return str if sz >= width

      diff = width - sz

      case diff
      when 1
        "#{str} "
      when 2
        "#{str}  "
      when 3
        "#{str}   "
      when 4
        "#{str}    "
      else
        str + (" " * diff)
      end
    end

    def space_expand(str, width)
      width = Integer(width)
      return space_expand_left(str, -width) if width < 0

      sz = str.size

      return str if sz >= width

      diff = width - sz

      case diff
      when 1
        " #{str}"
      when 2
        "  #{str}"
      when 3
        "   #{str}"
      when 4
        "    #{str}"
      else
        (" " * diff) << str
      end
    end

    class Builder
      def initialize(code, format, g)
        @code, @format = code, format

        @append_parts = 0

        @g = g
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
          @g.local_names = (0...@arg_count).map { |i| :"#{i + 1}$" } + [:splat]
        else
          @g.local_names = (0...@arg_count).map { |i| :"arg#{i}" } + [:splat]
        end

        @g.string_build @append_parts
        @g.ret
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

      RADIX = { 'u' => 10, 'x' => 16, 'X' => 16, 'o' => 8, 'b' => 2, 'B' => 2 }
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

      def append_literal(str)
        @g.push_unique_literal str
        append_str
      end

      def append_str
        @append_parts += 1
      end

      class Atom
        def initialize(b, g, format_code, flags, name = nil)
          @b, @g = b, g
          @format_code, @flags = format_code, flags
          @name = name

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
          @g.push_literal @prefix
          @g.string_dup
          @g.string_append
        end

        def set_value(ref)
          unless @name
            @field_index = @b.next_index(ref)
          end
        end

        def set_width(full, ref, static)
          @width_static = static && static.to_i
          if !@name && full && !static
            @width_index = @b.next_index(ref)
          end

          @has_width = @width_static || @width_index
        end

        def set_precision(full, ref, static)
          @prec_static = static && static.to_i
          if !@name && full && !static
            @prec_index = @b.next_index(ref)
          end

          @has_precision = @prec_static || @prec_index
        end

        def push_value
          if @name
            @g.push_local 0
            @g.push_unique_literal @name
            @g.send(:fetch, 1)
          else
            @g.push_local @field_index
          end
        end

        def push_width_value
          if @width_static
            @g.push @width_static
          elsif
            @g.push_local @width_index
          else
            raise "No width to push"
          end
        end

        def push_width(adjust=true)
          if @width_static
            raise ArgumentError, "width too big" unless @width_static.class == Fixnum
            if adjust && @full_leader_size > 0
              @g.push(@width_static - @full_leader_size)
            else
              @g.push @width_static
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

          else
            raise "push without a width"

          end
        end

        def push_precision_value
          if @prec_static
            @g.push @prec_static
          else
            @g.push_local @prec_index
          end
        end

        def push_precision
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
        end

        def push_format_string
          float_format_code = @format_code

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

        def zero_pad(pad="0")
          if @has_precision
            push_precision
          elsif @has_width && @f_zero
            push_width true
          else
            # exit early if no width has been pushed
            return
          end

          # let the caller adjust the width if needed
          yield if block_given?

          @g.push_literal pad
          @g.send :rjust, 2
        end

        attr_reader :width_static

        def leader?
          @full_leader_size > 0
        end
      end

      class StringAtom < Atom
        def string_justify
          # by default, f_space changes the padding introduced
          # by justify_width, but for string, we want to go
          # ahead and actually ignore it not have it count
          # against the justification space width.
          if @f_space
            @full_leader_size -= 1
          end

          justify_width

          if @has_precision
            @g.meta_push_0
            push_precision
            @g.send :[], 2
          end
        end

        def bytecode
          push_value
          @b.force_type :String

          string_justify

          @b.append_str
        end
      end

      class InspectAtom < StringAtom
        def bytecode
          push_value
          @g.send :inspect, 0

          string_justify

          @b.append_str
        end
      end

      class FloatAtom < Atom
        def bytecode
          push_value
          @b.force_type :Float

          format_done = @g.new_label

          @g.dup
          @g.send :finite?, 0

          @b.if_true do
            push_format_string
            @g.send :to_s_formatted, 1, true

            @g.goto format_done
          end

          formatted_non_finite = @g.new_label

          @g.dup
          @g.send :nan?, 0

          @b.if_false do
            @b.is_negative

            @b.if_false do
              @g.push_literal "#{positive_sign}Inf"
              @g.goto formatted_non_finite
            end
            @g.push_literal '-Inf'
            @g.goto formatted_non_finite
          end

          @g.pop
          @g.push_literal 'NaN'

          formatted_non_finite.set!
          justify_width false

          format_done.set!

          @b.append_str
        end
      end

      class IntegerAtom < Atom
        def bytecode
          if fast_common_case?
            @g.push :self

            push_value

            if @width_static == 2
              @g.send :zero_two_expand_integer, 1
            else
              @g.push_int @width_static
              @g.send :zero_expand_integer, 2
            end

            @b.append_str
          else
            @g.push :self
            push_value
            @g.send :as_int, 1

            val_idx = @g.new_stack_local
            @g.set_stack_local val_idx
            @g.pop

            # generic case

            if @f_space
              @g.push :self
              @g.push_stack_local val_idx
              @g.send :compute_space, 1
              @b.append_str
            elsif @f_plus
              @g.push :self
              @g.push_stack_local val_idx
              @g.send :compute_plus, 1
              @b.append_str
            end

            if @has_precision
              @g.push :self
              @g.push_stack_local val_idx

              push_precision_value

              @g.send :digit_expand_precision, 2

              if @has_width
                @g.push :self
                @g.swap
                push_width_value

                if @f_ljust
                  @g.send :space_expand_left, 2
                else
                  @g.send :space_expand, 2
                end
              end

              @b.append_str

            elsif @has_width
              @g.push :self
              @g.push_stack_local val_idx

              push_width_value

              expand_with_width

              @b.append_str
            else
              @g.push_stack_local val_idx
              @g.send :to_s, 0

              @b.append_str
            end
          end
        end
      end

      class ExtIntegerAtom < Atom
        def format_negative_int(radix)
          # (num + radix ** num.to_s(radix).size).to_s(radix)
          @g.push radix
          @g.dup_many 2
          @g.send :to_s, 1
          @g.send :size, 0
          @g.send :**, 1
          @g.meta_send_op_plus @g.find_literal(:+)
          @g.push radix
          @g.send :to_s, 1

          (radix - 1).to_s(radix)
        end

        def bytecode
          radix = RADIX[@format_code]

          push_value

          # Bignum is obviously also perfectly acceptable. But we
          # just address the most common case by avoiding the call
          # if we've been given a Fixnum. The call is enough
          # overhead to bother, but not something to panic about.
          @b.force_type :Fixnum, :Integer


          if @f_plus || @f_space
            @g.dup

            # stash away whether it's negative
            @b.is_negative
            @g.dup
            @g.move_down 2

            @b.if_true do
              # but treat it as positive for now
              @b.invert
            end

            @g.push radix
            @g.send :to_s, 1
          else
            have_formatted = @g.new_label

            @g.dup
            @b.is_negative

            @b.if_false do
              @g.push radix
              @g.send :to_s, 1
              @g.goto have_formatted
            end

            padding = format_negative_int(radix)
            pad_negative_int(padding)

            have_formatted.set!
          end

          # 'B' also returns an uppercase string, but there, the
          # only alpha character is in the prefix -- and that's
          # already uppercase
          if @format_code == 'X'
            @g.send :upcase, 0
          end

          zero_pad

          if @prefix
            prepend_prefix_bytecode
          end

          if @f_plus || @f_space
            append_sign = @g.new_label

            @g.swap
            @b.if_true do
              @g.push_literal '-'

              @g.goto append_sign
            end

            @g.push_literal positive_sign

            append_sign.set!
            @g.string_dup
            @g.string_append
          end


          if @has_precision || !@f_zero
            justify_width false
          end

          @b.append_str
        end
      end

      class LiteralAtom < Atom
        def set_value(ref)
          @value = ref
        end

        def bytecode
          @b.append_literal(@value)
        end
      end

      def push_Kernel
        @g.push_const :Kernel
      end

      def push_Fixnum
        @g.push_const :Fixnum
      end

      def push_String
        @g.push_const :String
      end

      def push_Hash
        @g.push_const :Hash
      end

      def raise_ArgumentError(msg)
        @lit_new ||= @g.add_literal(:new)

        @g.push_const :ArgumentError
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

      def try_type(klass, method)

        @lit_check ||= @g.add_literal(:check_convert_type)

        @g.dup
        @g.push_const klass
        @g.swap
        @g.kind_of
        if_false do
          @g.push_type
          @g.swap
          @g.push_const klass
          @g.push_unique_literal method
          @g.send_stack @lit_check, 3
          @g.dup
          if_false do
            yield if block_given?
          end
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

      AtomMap = Rubinius::LookupTable.new
      AtomMap[?s] = StringAtom
      AtomMap[?p] = InspectAtom
      AtomMap[?e] = AtomMap[?E] = FloatAtom
      AtomMap[?g] = AtomMap[?G] = FloatAtom
      AtomMap[?f] = FloatAtom
      AtomMap[?d] = AtomMap[?i] = IntegerAtom
      AtomMap[?b] = AtomMap[?B] = ExtIntegerAtom
      AtomMap[?x] = AtomMap[?X] = ExtIntegerAtom
      AtomMap[?o] = ExtIntegerAtom

      def parse
        @arg_count = 0
        @index_mode = nil

        atoms = []

        # Always push an empty string at first for correct
        # encoding protocols
        atom = LiteralAtom.new(self, @g, "", "")
        atom.set_value(encode_value(""))

        atoms << atom
        pos = 0
        while match = RE.match_start(@format, pos)
          pos = match.full.at(1)

          _,
          plain_string,
          whole_format,
          name_format,
          flags_a,
          field_ref_a,
          flags_b,
          width_full, width_ref, width_static,
          prec_full, prec_ref, prec_static,
          field_ref_b,
          format_code,
          literal_char,
          name_reference,
          invalid_format = *match

          flags = "#{flags_a}#{flags_b}"


          if plain_string
            atom = LiteralAtom.new(self, @g, format_code, flags)
            atom.set_value(plain_string)
            atoms << atom
          elsif literal_char
            atom = LiteralAtom.new(self, @g, format_code, flags)
            atom.set_value(literal_char)
            atoms << atom
          elsif invalid_format || (field_ref_a && field_ref_b)
            raise ArgumentError, "malformed format string: #{@format.inspect}"
          else
            field_ref = field_ref_a || field_ref_b

            if name_reference
              format_code = "s"
              @index_mode = :name
              name = name_reference[2...-1].to_sym
            elsif name_format
              @index_mode = :name
              name = name_format[1...-1].to_sym
            end

            klass = AtomMap[format_code[0]]

            unless klass
              raise ArgumentError, "unknown format type - #{format_code}"
            end

            atom = klass.new(self, @g, format_code, flags, name)
            atom.set_width width_full, width_ref, width_static
            atom.set_precision prec_full, prec_ref, prec_static
            atom.set_value field_ref
            atoms << atom
          end
        end

        if @index_mode == :name
          exception = @g.new_label
          continue  = @g.new_label

          @arg_count = 1
          @g.passed_arg @arg_count
          @g.git exception
          push_Hash
          @g.push_local 0
          @g.kind_of
          @g.gif exception
          @g.goto continue

          exception.set!

          raise_ArgumentError "named format string needs single Hash argument"

          continue.set!
        elsif @index_mode != :absolute
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

        atoms.each do |atom|
          atom.bytecode
        end
      end
    end
  end
end
