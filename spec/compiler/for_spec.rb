require File.expand_path('../../spec_helper', __FILE__)

describe "A For node" do
  relates <<-ruby do
      for o in ary do
        puts(o)
      end
    ruby

    compile do |g|
      g.push :self
      g.send :ary, 0, true

      d = new_block_generator(g)

      d.cast_for_single_block_arg
      d.set_local_depth 1, 0

      d.pop
      d.push_modifiers
      d.new_label.set!

      d.push :self
      d.push_local_depth 1, 0
      d.send :puts, 1, true

      d.pop_modifiers
      d.ret

      g.create_block(d)

      g.send_with_block :each, 0, false
    end
  end

  relates <<-ruby do
      for i in (0..max) do
        # do nothing
      end
    ruby

    compile do |g|
      g.push_cpath_top
      g.find_const :Range
      g.push 0
      g.push :self
      g.send :max, 0, true
      g.send :new, 2

      d = new_block_generator(g)

      d.cast_for_single_block_arg
      d.set_local_depth 1, 0

      d.pop
      d.push_modifiers
      d.new_label.set!

      d.push :nil

      d.pop_modifiers
      d.ret

      g.create_block(d)

      g.send_with_block :each, 0, false
    end
  end

  relates <<-ruby do
      for a, b in x do
        5
      end
    ruby

    compile do |g|
      d = new_block_generator(g)

      d.cast_for_multi_block_arg
      d.shift_array
      d.set_local_depth 1, 0
      d.pop
      d.shift_array
      d.set_local_depth 1, 1
      d.pop
      d.pop
      d.push_modifiers
      d.new_label.set!
      d.push 5
      d.pop_modifiers
      d.ret

      g.push :self
      g.send :x, 0, true
      g.create_block d

      g.send_with_block :each, 0, false
    end
  end

  relates <<-ruby do
      for i in ()
        i
      end
    ruby

    compile do |g|
      g.push :nil

      d = new_block_generator(g)

      d.cast_for_single_block_arg
      d.set_local_depth 1, 0

      d.pop
      d.push_modifiers
      d.new_label.set!

      d.push_local_depth 1, 0

      d.pop_modifiers
      d.ret

      g.create_block(d)

      g.send_with_block :each, 0, false
    end
  end

  relates <<-ruby do
      c = 1
      for i in a
        for j in b
          c
        end
      end
    ruby

    compile do |g|
      g.push 1
      g.set_local 0
      g.pop

      g.push :self
      g.send :a, 0, true

      # outer for
      d = new_block_generator(g)

      d.cast_for_single_block_arg
      d.set_local_depth 1, 1
      d.pop

      d.push_modifiers
      d.new_label.set!

      d.push :self
      d.send :b, 0, true

      # inner for
      e = new_block_generator(g)

      e.cast_for_single_block_arg
      e.set_local_depth 2, 2
      e.pop

      e.push_modifiers
      e.new_label.set!
      e.push_local_depth 2, 0

      e.pop_modifiers
      e.ret

      d.create_block(e)
      d.send_with_block :each, 0, false

      d.pop_modifiers
      d.ret

      g.create_block(d)
      g.send_with_block :each, 0, false
    end
  end
end
