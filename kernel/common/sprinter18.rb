# -*- encoding: us-ascii -*-

module Rubinius
  class Sprinter

    class Builder

      def encode_value(str)
        str
      end

      class Atom
        def zero_pad?
          @has_precision || (@has_width && @f_zero)
        end
      end

      class CharAtom < Atom
        def bytecode
          push_value
          @b.force_type :Fixnum, :Integer

          chr_range_ok = @g.new_label

          @g.dup
          @g.push 256
          @g.meta_send_op_lt @g.find_literal(:<)
          @b.if_true do
            @g.dup
            @g.meta_push_neg_1
            @g.meta_send_op_gt @g.find_literal(:>)
            @g.git chr_range_ok
          end

          @g.push 256
          @g.send :%, 1

          chr_range_ok.set!
          @g.send :chr, 0

          justify_width

          @b.append_str
        end
      end

      AtomMap[?c] = CharAtom

      class IntegerAtom < Atom
        def fast_common_case?
          @f_zero && @width_static && !@has_precision && !@f_space && !@f_plus
        end

        def expand_with_width
          if @f_zero
            if @f_space || @f_plus
              @g.send :zero_expand_leader, 2
            else
              @g.send :zero_expand_integer, 2
            end
          else
            if @f_space || @f_plus
              if @f_ljust
                @g.send :space_expand_leader_left, 2
              else
                @g.send :space_expand_leader, 2
              end
            else
              if @f_ljust
                @g.send :space_expand_integer_left, 2
              else
                @g.send :space_expand_integer, 2
              end
            end
          end
        end
      end

      class ExtIntegerAtom < Atom
        def pad_negative_int(padding)
          if zero_pad?
            zero_pad(padding)

          elsif !@has_precision && !@f_zero
            @g.push_literal ".."
            @g.string_dup
            @g.string_append
          end
        end

        def prepend_prefix_bytecode
          prepend_prefix
        end
      end

      class ExtIntegerAtomU < ExtIntegerAtom
        def format_negative_int(radix)
          # Now we need to find how many bits we need to
          # represent the number, starting with a native int,
          # then incrementing by 32 each round.

          more_bits_loop = @g.new_label
          got_enough_bits = @g.new_label

          # Push a positive version of the number ($N)
          @g.dup
          @b.invert

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

          "."
        end
      end

      AtomMap[?u] = ExtIntegerAtomU

      RE = /
        ([^%]+|%(?:[\n\0]|\z)) # 1
        |
        %
        ( # 2
          (.\A)?
          ([0# +-]*) # 3
          (?:([0-9]+)\$)? # 4
          ([0# +-]*) # 5
          (?:
            (\*(?:([0-9]+)\$)?|([1-9][0-9]*))? # 6 7 8
            (?:\.(\*(?:([0-9]+)\$)?|([0-9][0-9]*))?)? # 9 10 11
          )
          (?:([0-9]+)\$)? # 12
          ([BbcdEefGgiopsuXx]) # 13
        )
        |
        (%)(?:%|[-+0-9# *.$]+\$[0-9.]*\z) # 14
        |
        (.\A)
        |
        (%) # 15
      /x

    end
  end
end
