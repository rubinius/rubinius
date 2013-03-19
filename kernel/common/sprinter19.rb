# -*- encoding: us-ascii -*-

module Rubinius
  class Sprinter

    class Builder

      def encode_result
        @g.push_literal @format.encoding
        @g.send :force_encoding, 1
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
