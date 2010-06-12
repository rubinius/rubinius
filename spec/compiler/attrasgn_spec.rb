require File.expand_path('../../spec_helper', __FILE__)

describe "An Attrasgn node" do
  relates <<-ruby do
      y = 0
      42.method = y
    ruby

    compile do |g|
      g.push 0
      g.set_local 0
      g.pop
      g.push 42
      g.push_local 0
      g.dup
      g.move_down 2
      g.send :method=, 1, false
      g.pop
    end
  end

  relates "a.m = *[1]" do
    # attrasgn_splat
  end

  relates "a[*b] = c" do
    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.push :self
      g.send :b, 0, true
      g.cast_array
      g.push :self
      g.send :c, 0, true
      g.dup
      g.move_down 3
      g.swap
      g.push :nil
      g.send_with_splat :[]=, 1, false, true
      g.pop
    end
  end

  relates "a[b, *c] = d" do
    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.push :self
      g.send :b, 0, true
      g.make_array 1
      g.push :self
      g.send :c, 0, true
      g.cast_array
      g.send :+, 1
      g.push :self
      g.send :d, 0, true
      g.dup
      g.move_down 3
      g.send :[]=, 2, false
      g.pop
    end
  end

  relates "a[b, *c] = *d" do
    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.push :self
      g.send :b, 0, true
      g.make_array 1
      g.push :self
      g.send :c, 0, true
      g.cast_array
      g.send :+, 1
      g.push :self
      g.send :d, 0, true
      g.cast_array
      g.dup
      g.send :size, 0
      g.push 1
      g.send :>, 1

      bigger = g.new_label
      g.git bigger
      g.push 0
      g.send :at, 1

      bigger.set!
      g.dup
      g.move_down 3
      g.send :[]=, 2, false
      g.pop
    end
  end

  relates "a[b, *c] = d, e" do
    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.push :self
      g.send :b, 0, true
      g.make_array 1
      g.push :self
      g.send :c, 0, true
      g.cast_array
      g.send :+, 1
      g.push :self
      g.send :d, 0, true
      g.push :self
      g.send :e, 0, true
      g.make_array 2
      g.dup
      g.move_down 3
      g.send :[]=, 2, false
      g.pop
    end
  end

  relates "a[42] = 24" do
    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.push 42
      g.push 24
      g.dup
      g.move_down 3
      g.send :[]=, 2, false
      g.pop
    end
  end

  relates "self[index, 0] = other_string" do
    compile do |g|
      g.push :self
      g.push :self
      g.send :index, 0, true
      g.push 0
      g.push :self
      g.send :other_string, 0, true
      g.dup
      g.move_down 4
      g.send :[]=, 3, true
      g.pop
    end
  end

  relates <<-ruby do
      a = []
      a [42] = 24
    ruby

    compile do |g|
      g.make_array 0
      g.set_local 0
      g.pop
      g.push_local 0
      g.push 42
      g.push 24
      g.dup
      g.move_down 3
      g.send :[]=, 2, false
      g.pop
    end
  end

  relates "m { |a.b| }" do
    compile do |g|
      g.push :self

      d = new_block_generator(g)

      d.cast_for_single_block_arg

      d.push :self
      d.send :a, 0, true
      d.swap
      d.send :b=, 1, false
      d.pop

      d.push_modifiers
      d.new_label.set!
      d.push :nil
      d.pop_modifiers
      d.ret

      g.create_block(d)

      g.send_with_block :m, 0, true
    end
  end
end
