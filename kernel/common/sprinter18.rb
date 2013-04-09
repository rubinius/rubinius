# -*- encoding: us-ascii -*-

module Rubinius
  class Sprinter

    class Builder

      def encode_result
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
          @f_zero and @width_static and !@f_space and !@f_plus
        end

        def expand_with_width
          if @f_zero
            if @f_space or @f_plus
              @g.send :zero_expand_leader, 2
            else
              @g.send :zero_expand_integer, 2
            end
          else
            if @f_space or @f_plus
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
