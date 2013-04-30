require File.expand_path('../../spec_helper', __FILE__)

describe "A Masgn node" do
  relates "a, b.c = b.c, true" do
    compile do |g|
      g.push :self
      g.send :b, 0, true
      g.send :c, 0, false
      g.push :true

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.rotate 2

      g.set_local 0
      g.pop

      g.push :self
      g.send :b, 0, true
      g.swap
      g.send :c=, 1, false

      g.pop
    end
  end

  relates "a, b = 1, 2, 3" do
    compile do |g|
      g.push 1
      g.push 2
      g.push 3

      g.dup_many 3
      g.make_array 3
      g.move_down 3

      g.rotate 3

      g.set_local 0
      g.pop
      g.set_local 1
      g.pop

      g.pop # no set_local since the LHS is smaller than the RHS
    end
  end

  relates "a, b = c, d" do
    compile do |g|
      g.push :self
      g.send :c, 0, true
      g.push :self
      g.send :d, 0, true

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.rotate 2

      g.set_local 0
      g.pop

      g.set_local 1
      g.pop
    end
  end

  ruby_version_is ""..."1.9" do
    relates "a, b, *c = 1, 2, *[3, 4]" do
      compile do |g|
        g.push 1
        g.push 2
        g.make_array 2
        g.push 3
        g.push 4
        g.make_array 2
        g.cast_array
        g.send :+, 1
        g.cast_array
        g.dup

        g.shift_array
        g.set_local 0
        g.pop

        g.shift_array
        g.set_local 1
        g.pop

        g.cast_array
        g.set_local 2
        g.pop
      end
    end
  end

  ruby_version_is "1.9" do
    relates "a, b, *c = 1, 2, *[3, 4]" do
      compile do |g|
        g.push 1
        g.push 2
        g.push 3
        g.push 4

        g.dup_many 4
        g.make_array 4
        g.move_down 4

        g.make_array 2
        g.rotate 3

        g.set_local 0
        g.pop

        g.set_local 1
        g.pop

        g.cast_array
        g.set_local 2
        g.pop
      end
    end
  end

  relates "a.b, a.c, _ = q" do
    compile do |g|
      g.push :self
      g.send :q, 0, true
      g.cast_multi_value

      g.dup

      g.shift_array
      g.push :self
      g.send :a, 0, true
      g.swap
      g.send :b=, 1, false
      g.pop

      g.shift_array
      g.push :self
      g.send :a, 0, true
      g.swap
      g.send :c=, 1, false
      g.pop

      g.shift_array
      g.set_local 0
      g.pop

      g.pop
    end
  end

  relates <<-ruby do
      a, i, j = [], 1, 2
      a[i], a[j] = a[j], a[i]
    ruby

    compile do |g|
      g.make_array 0
      g.push 1
      g.push 2

      g.dup_many 3
      g.make_array 3
      g.move_down 3

      g.rotate 3

      g.set_local 0
      g.pop

      g.set_local 1
      g.pop

      g.set_local 2
      g.pop

      g.pop

      g.push_local 0
      g.push_local 2
      g.send :[], 1, false
      g.push_local 0
      g.push_local 1
      g.send :[], 1, false

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.rotate 2

      g.push_local 0
      g.swap
      g.push_local 1
      g.swap
      g.send :[]=, 2, false
      g.pop

      g.push_local 0
      g.swap
      g.push_local 2
      g.swap
      g.send :[]=, 2, false
      g.pop
    end
  end

  relates <<-ruby do
      c, d, e, f = [], 1, 2, 3
      a, *b = c[d] = f(e, f, c)
    ruby

    compile do |g|
      g.make_array 0
      g.push 1
      g.push 2
      g.push 3

      g.dup_many 4
      g.make_array 4
      g.move_down 4

      g.rotate 4

      g.set_local 0
      g.pop
      g.set_local 1
      g.pop
      g.set_local 2
      g.pop
      g.set_local 3
      g.pop

      g.pop

      g.push_local 0
      g.push_local 1
      g.push :self
      g.push_local 2
      g.push_local 3
      g.push_local 0
      g.send :f, 3, true

      g.dup
      g.move_down 3
      g.send :[]=, 2, false
      g.pop

      g.cast_multi_value
      g.dup

      g.shift_array
      g.set_local 4
      g.pop
      g.cast_array
      g.set_local 5
      g.pop
    end
  end

  relates "a, b.c = d, e" do
    compile do |g|
      g.push :self
      g.send :d, 0, true
      g.push :self
      g.send :e, 0, true

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.rotate 2

      g.set_local 0
      g.pop

      g.push :self
      g.send :b, 0, true
      g.swap
      g.send :c=, 1, false

      g.pop
    end
  end

  relates "*a.m = *b" do
    compile do |g|
      g.push :self
      g.send :b, 0, true
      g.cast_array
      g.dup

      g.push :self
      g.send :a, 0, true
      g.swap
      g.send :m=, 1, false

      g.pop
    end
  end

  relates "*a.m = b" do
    compile do |g|
      g.push :self
      g.send :b, 0, true

      g.make_array 1
      g.push :self
      g.send :a, 0, true
      g.swap
      g.send :m=, 1, false

      g.dup
      g.pop
    end
  end

  relates "t, *a[:m] = b" do
    compile do |g|
      ary = g.new_label
      assign = g.new_label

      g.push :self
      g.send :b, 0, true

      g.cast_multi_value
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.cast_array
      g.push :self
      g.send :a, 0, true
      g.swap
      g.push_literal :m
      g.swap
      g.send :[]=, 2, false
      g.pop
    end
  end

  relates "A, B, C = 1, 2, 3" do
    compile do |g|
      g.push 1
      g.push 2
      g.push 3

      g.dup_many 3
      g.make_array 3
      g.move_down 3

      g.rotate 3

      g.push_scope
      g.swap
      g.push_literal :A
      g.swap
      g.send :const_set, 2
      g.pop

      g.push_scope
      g.swap
      g.push_literal :B
      g.swap
      g.send :const_set, 2
      g.pop

      g.push_scope
      g.swap
      g.push_literal :C
      g.swap
      g.send :const_set, 2
      g.pop
    end
  end

  relates "* = 1, 2" do
    compile do |g|
      g.push 1
      g.push 2

      g.make_array 2
      g.dup

      g.pop
    end
  end

  relates "*$a = b" do
    compile do |g|
      g.push :self
      g.send :b, 0, true
      g.make_array 1
      g.push_rubinius
      g.find_const :Globals
      g.swap
      g.push_literal :$a
      g.swap
      g.send :[]=, 2
      g.dup
      g.pop
    end
  end

  relates "*$a = *b" do
    compile do |g|
      g.push :self
      g.send :b, 0, true
      g.cast_array
      g.dup
      g.push_rubinius
      g.find_const :Globals
      g.swap
      g.push_literal :$a
      g.swap
      g.send :[]=, 2
      g.pop
    end
  end

  relates "a, @b = c, d" do
    compile do |g|
      g.push :self
      g.send :c, 0, true
      g.push :self
      g.send :d, 0, true

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.rotate 2

      g.set_local 0
      g.pop

      g.set_ivar :@b
      g.pop
    end
  end

  relates "*@a = b" do
    compile do |g|
      ary = g.new_label
      assign = g.new_label

      g.push :self
      g.send :b, 0, true

      g.make_array 1
      g.set_ivar :@a
      g.dup

      g.pop
    end
  end

  relates "*@a = *b" do
    compile do |g|
      g.push :self
      g.send :b, 0, true
      g.cast_array
      g.dup
      g.set_ivar :@a
      g.pop
    end
  end

  relates "@a, $b = 1, 2" do
    compile do |g|
      g.push 1
      g.push 2

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.rotate 2

      g.set_ivar :@a
      g.pop

      g.push_rubinius
      g.find_const :Globals
      g.swap
      g.push_literal :$b
      g.swap
      g.send :[]=, 2

      g.pop
    end
  end

  relates "a, b = (@a = 1), @a" do
    compile do |g|
      g.push 1
      g.set_ivar :@a
      g.push_ivar :@a

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.rotate 2

      g.set_local 0
      g.pop
      g.set_local 1
      g.pop
    end
  end

  relates "@@a, @@b = 1, 2" do
    compile do |g|
      g.push 1
      g.push 2

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.rotate 2

      g.push_scope
      g.swap
      g.push_literal :@@a
      g.swap
      g.send :class_variable_set, 2
      g.pop

      g.push_scope
      g.swap
      g.push_literal :@@b
      g.swap
      g.send :class_variable_set, 2
      g.pop
    end
  end

  relates "a, b, *c = 1, 2, 3" do
    compile do |g|
      g.push 1
      g.push 2
      g.push 3

      g.dup_many 3
      g.make_array 3
      g.move_down 3

      g.make_array 1

      g.rotate 3

      g.set_local 0
      g.pop

      g.set_local 1
      g.pop

      g.cast_array
      g.set_local 2
      g.pop
    end
  end

  relates "a, b, *c = 1, 2" do
    compile do |g|
      g.push 1
      g.push 2

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.make_array 0

      g.rotate 3

      g.set_local 0
      g.pop

      g.set_local 1
      g.pop

      g.cast_array
      g.set_local 2
      g.pop
    end
  end

  relates "a, b, c, *d = 1, 2" do
    compile do |g|
      g.push 1
      g.push 2

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.push :nil
      g.make_array 0

      g.rotate 4

      g.set_local 0
      g.pop

      g.set_local 1
      g.pop

      g.set_local 2
      g.pop

      g.cast_array
      g.set_local 3
      g.pop
    end
  end

  relates "a, b, c = *d" do
    compile do |g|
      g.push :self
      g.send :d, 0, true

      g.cast_array
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.set_local 1
      g.pop

      g.shift_array
      g.set_local 2
      g.pop

      g.pop
    end
  end

  relates "a, b, c = 1, *d" do
    compile do |g|
      g.push 1
      g.make_array 1

      g.push :self
      g.send :d, 0, true
      g.cast_array

      g.send :+, 1
      g.cast_array

      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.set_local 1
      g.pop

      g.shift_array
      g.set_local 2
      g.pop

      g.pop
    end
  end

  relates "a, b, *c = *d" do
    compile do |g|
      g.push :self
      g.send :d, 0, true

      g.cast_array
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.set_local 1
      g.pop

      g.cast_array
      g.set_local 2
      g.pop
    end
  end

  relates "*a = 1, 2, 3" do
    compile do |g|
      g.push 1
      g.push 2
      g.push 3

      g.make_array 3
      g.set_local 0
      g.dup

      g.pop
    end
  end

  relates "*a = b" do
    compile do |g|
      g.push :self
      g.send :b, 0, true

      g.make_array 1
      g.set_local 0
      g.dup

      g.pop
    end
  end

  relates "*a = *b" do
    compile do |g|
      g.push :self
      g.send :b, 0, true
      g.cast_array
      g.dup
      g.set_local 0
      g.pop
    end
  end

  relates "a, (b, c) = [1, [2, 3]]" do
    compile do |g|
      g.push 1
      g.push 2
      g.push 3
      g.make_array 2
      g.make_array 2
      g.cast_multi_value
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.cast_array
      g.shift_array
      g.set_local 1
      g.pop

      g.shift_array
      g.set_local 2
      g.pop

      g.pop
      g.pop
    end
  end

  relates "a, = *[[[1]]]" do
    compile do |g|
      g.push 1
      g.make_array 1
      g.make_array 1
      g.make_array 1
      g.dup
      g.shift_array
      g.set_local 0
      g.pop
      g.pop
    end
  end

  relates "a, b, * = c" do
    compile do |g|
      g.push :self
      g.send :c, 0, true
      g.cast_multi_value
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.set_local 1
      g.pop

      g.pop
    end
  end

  relates "a, b, = c" do
    compile do |g|
      g.push :self
      g.send :c, 0, true
      g.cast_multi_value
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.set_local 1
      g.pop

      g.pop
    end
  end

  relates "a, b, c = m d" do
    compile do |g|
      g.push :self
      g.push :self
      g.send :d, 0, true
      g.send :m, 1, true
      g.cast_multi_value
      g.dup
      g.shift_array
      g.set_local 0
      g.pop
      g.shift_array
      g.set_local 1
      g.pop
      g.shift_array
      g.set_local 2
      g.pop
      g.pop
    end
  end

  relates "a, b, *c = d, e, f, g" do
    compile do |g|
      g.push :self
      g.send :d, 0, true
      g.push :self
      g.send :e, 0, true
      g.push :self
      g.send :f, 0, true
      g.push :self
      g.send :g, 0, true

      g.dup_many 4
      g.make_array 4
      g.move_down 4

      g.make_array 2

      g.rotate 3

      g.set_local 0
      g.pop

      g.set_local 1
      g.pop

      g.cast_array
      g.set_local 2
      g.pop
    end
  end

  relates "a, b, *c = d.e(\"f\")" do
    compile do |g|
      g.push :self
      g.send :d, 0, true
      g.push_literal "f"
      g.string_dup
      g.send :e, 1, false

      g.cast_multi_value
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.set_local 1
      g.pop

      g.cast_array
      g.set_local 2
      g.pop
    end
  end

  relates "a, b, *c = d" do
    compile do |g|
      g.push :self
      g.send :d, 0, true

      g.cast_multi_value
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.set_local 1
      g.pop

      g.cast_array
      g.set_local 2
      g.pop
    end
  end

  relates "a, b = c" do
    compile do |g|
      g.push :self
      g.send :c, 0, true
      g.cast_multi_value
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.set_local 1
      g.pop

      g.pop
    end
  end

  relates <<-ruby do
      m do
        a, b = 1, 2
        next
      end
    ruby

    compile do |g|
      g.push :self
      g.in_block_send :m, :none do |d|
        d.push 1
        d.push 2
        d.dup_many 2
        d.make_array 2
        d.move_down 2
        d.rotate 2
        d.set_local 0
        d.pop
        d.set_local 1
        d.pop
        d.pop
        d.push :nil
        d.ret
      end
    end
  end

  relates "a, (b, c) = 1" do
    compile do |g|
      g.push 1
      g.cast_multi_value
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.cast_array

      g.shift_array
      g.set_local 1
      g.pop

      g.shift_array
      g.set_local 2
      g.pop

      g.pop
      g.pop
    end
  end

  relates "a, (b, c) = *1" do
    compile do |g|
      g.push 1
      g.cast_array
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.cast_array

      g.shift_array
      g.set_local 1
      g.pop

      g.shift_array
      g.set_local 2
      g.pop

      g.pop
      g.pop
    end
  end

  relates "a, (b, c) = 1, 2, 3" do
    compile do |g|
      g.push 1
      g.push 2
      g.push 3

      g.dup_many 3
      g.make_array 3
      g.move_down 3

      g.rotate 3

      g.set_local 0
      g.pop

      g.cast_array

      g.shift_array
      g.set_local 1
      g.pop

      g.shift_array
      g.set_local 2
      g.pop

      g.pop
      g.pop
    end
  end

  relates "a, (b, *c), d = 1, 2, 3, 4" do
    compile do |g|
      g.push 1
      g.push 2
      g.push 3
      g.push 4

      g.dup_many 4
      g.make_array 4
      g.move_down 4

      g.rotate 4

      g.set_local 0
      g.pop

      g.cast_array

      g.shift_array
      g.set_local 1
      g.pop

      ary = g.new_label
      assign = g.new_label

      g.dup
      g.push_cpath_top
      g.find_const :Array
      g.swap
      g.kind_of
      g.git assign
      g.make_array 1

      assign.set!
      g.set_local 2
      g.pop

      g.set_local 3
      g.pop

      g.pop
    end
  end

  relates "a, (b, c) = 1, *2" do
    compile do |g|
      g.push 1
      g.make_array 1
      g.push 2
      g.cast_array
      g.send :+, 1
      g.cast_array
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.cast_array

      g.shift_array
      g.set_local 1
      g.pop

      g.shift_array
      g.set_local 2
      g.pop

      g.pop
      g.pop
    end
  end

  relates "a, (b, *c) = 1" do
    compile do |g|
      g.push 1
      g.cast_multi_value
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.cast_array

      g.shift_array
      g.set_local 1
      g.pop

      assign = g.new_label

      g.dup
      g.push_cpath_top
      g.find_const :Array
      g.swap
      g.kind_of
      g.git assign
      g.make_array 1

      assign.set!
      g.set_local 2

      g.pop
      g.pop
    end
  end

  relates "a, (b, *c) = 1, 2" do
    compile do |g|
      g.push 1
      g.push 2

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.rotate 2

      g.set_local 0
      g.pop

      g.cast_array

      g.shift_array
      g.set_local 1
      g.pop

      assign = g.new_label

      g.dup
      g.push_cpath_top
      g.find_const :Array
      g.swap
      g.kind_of
      g.git assign
      g.make_array 1

      assign.set!
      g.set_local 2

      g.pop
    end
  end

  relates "a, (b, *c) = *1" do
    compile do |g|
      g.push 1
      g.cast_array
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.cast_array

      g.shift_array
      g.set_local 1
      g.pop

      assign = g.new_label

      g.dup
      g.push_cpath_top
      g.find_const :Array
      g.swap
      g.kind_of
      g.git assign
      g.make_array 1

      assign.set!
      g.set_local 2
      g.pop

      g.pop
    end
  end

  relates "a, (b, *c) = 1, *2" do
    compile do |g|
      g.push 1
      g.make_array 1
      g.push 2
      g.cast_array
      g.send :+, 1

      g.cast_array
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array
      g.cast_array

      g.shift_array
      g.set_local 1
      g.pop

      assign = g.new_label

      g.dup
      g.push_cpath_top
      g.find_const :Array
      g.swap
      g.kind_of
      g.git assign
      g.make_array 1

      assign.set!
      g.set_local 2
      g.pop

      g.pop
    end
  end

  relates "a, (*b) = 1" do
    compile do |g|
      g.push 1
      g.cast_multi_value
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      assign = g.new_label

      g.shift_array
      g.dup
      g.push_cpath_top
      g.find_const :Array
      g.swap
      g.kind_of
      g.git assign
      g.make_array 1

      assign.set!
      g.set_local 1
      g.pop

      g.pop
    end
  end

  relates "a, (*b) = 1, 2" do
    compile do |g|
      g.push 1
      g.push 2

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.rotate 2

      g.set_local 0
      g.pop

      assign = g.new_label

      g.dup
      g.push_cpath_top
      g.find_const :Array
      g.swap
      g.kind_of
      g.git assign
      g.make_array 1

      assign.set!
      g.set_local 1
      g.pop
    end
  end

  relates "a, (*b) = *1" do
    compile do |g|
      g.push 1
      g.cast_array
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      assign = g.new_label

      g.shift_array
      g.dup
      g.push_cpath_top
      g.find_const :Array
      g.swap
      g.kind_of
      g.git assign
      g.make_array 1

      assign.set!
      g.set_local 1
      g.pop

      g.pop
    end
  end

  relates "a, (*b) = 1, *2" do
    compile do |g|
      g.push 1
      g.make_array 1
      g.push 2
      g.cast_array
      g.send :+, 1
      g.cast_array
      g.dup

      g.shift_array
      g.set_local 0
      g.pop

      g.shift_array

      assign = g.new_label

      g.dup
      g.push_cpath_top
      g.find_const :Array
      g.swap
      g.kind_of
      g.git assign
      g.make_array 1

      assign.set!
      g.set_local 1
      g.pop

      g.pop
    end
  end

  relates "a, b = nil, lambda { a = 7 }" do
    compile do |g|
      g.push :nil

      g.push :self
      g.in_block_send :lambda, :none do |d|
        d.push 7
        d.set_local_depth 1, 0
      end

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.rotate 2

      g.set_local 0
      g.pop

      g.set_local 1
      g.pop
    end
  end

  relates "a, *b = lambda { b = 8 }, nil" do
    compile do |g|
      g.push :self
      g.in_block_send :lambda, :none do |d|
        d.push 8
        d.set_local_depth 1, 1
      end

      g.push :nil

      g.dup_many 2
      g.make_array 2
      g.move_down 2

      g.make_array 1
      g.rotate 2

      g.set_local 0
      g.pop

      g.cast_array
      g.set_local 1
      g.pop
    end
  end

  relates "*a = lambda { a = 8 }" do
    compile do |g|
      g.push :self
      g.in_block_send :lambda, :none do |d|
        d.push 8
        d.set_local_depth 1, 0
      end

      g.make_array 1
      g.set_local 0
      g.dup

      g.pop
    end
  end

  relates "a, (b, *c), d = lambda { b = d }, [], nil" do
    compile do |g|
      g.push :self
      g.in_block_send :lambda, :none do |d|
        d.push_local_depth 1, 3
        d.set_local_depth 1, 1
      end

      g.make_array 0
      g.push :nil

      g.dup_many 3
      g.make_array 3
      g.move_down 3

      g.rotate 3

      g.set_local 0
      g.pop

      g.cast_array
      g.shift_array
      g.set_local 1
      g.pop
      g.dup
      g.push_cpath_top
      g.find_const :Array
      g.swap
      g.kind_of

      is_array = g.new_label
      g.git is_array
      g.make_array 1
      is_array.set!

      g.set_local 2
      g.pop
      g.set_local 3
      g.pop
    end
  end
end
