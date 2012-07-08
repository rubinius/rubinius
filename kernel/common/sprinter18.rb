# -*- encoding: us-ascii -*-

module Rubinius
  class Sprinter

    class Builder

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
    end
  end
end
