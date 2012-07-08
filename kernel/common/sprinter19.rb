# -*- encoding: us-ascii -*-

module Rubinius
  class Sprinter

    class Builder

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
    end
  end
end
