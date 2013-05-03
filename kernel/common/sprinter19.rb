# -*- encoding: us-ascii -*-

module Rubinius
  class Sprinter

    class Builder

      def encode_value(str)
        str.force_encoding(@format.encoding)
        str
      end

      def is_zero
        @g.meta_push_0
        @g.meta_send_op_equal @g.find_literal(:==)
      end

      class CharAtom < Atom
        def bytecode
          push_value
          # We need to push the original value here
          # because we have to possibly make multiple
          # attempts to coerce it to either a string
          # or an integer. If we don't duplicate we can't
          # retry it for another type with the same original
          # value.
          @g.dup

          @b.try_type :String, :to_str do
            # Attempt failed, pop the nil value
            # and retry for Integer
            @g.pop
            @g.dup
            @b.force_type :Fixnum, :Integer
            @g.send :chr, 0
          end

          @g.dup
          @g.send :length, 0
          @g.meta_push_1
          @g.meta_send_op_equal @g.find_literal(:==)
          @b.if_false do
            @b.raise_ArgumentError "%c requires a character"
          end

          justify_width

          # Swap the given value with the coerced value and
          # pop off the given value afterwards
          @g.swap
          @g.pop
          @b.append_str
        end
      end

      AtomMap[?c] = CharAtom

      class IntegerAtom < Atom
        def fast_common_case?
          @f_zero && @width_static && !@has_precision && !@f_space && !@f_plus && !@f_ljust
        end

        def expand_with_width
          if @f_ljust
            if @f_space || @f_plus
              @g.send :space_expand_leader_left, 2
            else
              @g.send :space_expand_integer_left, 2
            end
          else
            if @f_zero
              if @f_space || @f_plus
                @g.send :zero_expand_leader, 2
              else
                @g.send :zero_expand_integer, 2
              end
            else
              if @f_space || @f_plus
                @g.send :space_expand_leader, 2
              else
                @g.send :space_expand_integer, 2
              end
            end
          end
        end
      end

      class ExtIntegerAtom < Atom
        def pad_negative_int(padding)
          zero_pad(padding) do
            # decrease the width by 2 to account for the ".." below
            @g.meta_push_2
            @b.meta_op_minus
          end

          @g.push_literal ".."
          @g.string_dup
          @g.string_append
        end

        def prepend_prefix_bytecode
          skip_prefix = @g.new_label

          # For 'x' and 'X', don't prepend the "0x" prefix
          # if the value is zero
          if @format_code == 'x' || @format_code == 'X'
            push_value
            @b.is_zero
            @g.git skip_prefix
          end

          prepend_prefix

          skip_prefix.set!
        end
      end

      AtomMap[?u] = IntegerAtom

      RE = /
        ([^%]+|%(?:[\n\0]|\z)) # 1
        |
        %
        ( # 2
          (<[^>]+>)? # 3
          ([0# +-]*) # 4
          (?:([0-9]+)\$)? # 5
          ([0# +-]*) # 6
          (?:
            (\*(?:([0-9]+)\$)?|([1-9][0-9]*))? # 7 8 9
            (?:\.(\*(?:([0-9]+)\$)?|([0-9][0-9]*))?)? # 10 11 12
          )
          (?:([0-9]+)\$)? # 13
          ([BbcdEefGgiopsuXx]) # 14
        )
        |
        (%)(?:%|[-+0-9# *.$]+\$[0-9.]*\z) # 15
        |
        (%{[^}]+}) #16
        |
        (%) # 17
      /x

    end
  end
end
