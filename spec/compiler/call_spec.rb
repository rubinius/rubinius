require File.expand_path('../../spec_helper', __FILE__)

describe "A Call node" do
  relates "self.method" do
    compile do |g|
      g.push :self
      g.send :method, 0, false
    end
  end

  relates "1.m(2)" do
    compile do |g|
      g.push 1
      g.push 2
      g.send :m, 1, false
    end
  end

  relates "h(1, 2, *a)" do
    compile do |g|
      g.push :self
      g.push 1
      g.push 2
      g.push :self
      g.send :a, 0, true
      g.cast_array

      g.push :nil
      g.send_with_splat :h, 2, true, false
    end
  end

  relates <<-ruby do
      begin
        (1 + 1)
      end
    ruby

    compile do |g|
      g.push 1
      g.push 1
      g.send :+, 1, false
    end
  end

  relates "blah(*a)" do
    compile do |g|
      g.push :self
      g.push :self
      g.send :a, 0, true
      g.cast_array
      g.push :nil
      g.send_with_splat :blah, 0, true, false
    end
  end

  relates "a.b(&c)" do
    compile do |g|
      t = g.new_label

      g.push :self
      g.send :a, 0, true
      g.push :self
      g.send :c, 0, true

      g.dup
      g.is_nil
      g.git t

      g.push_cpath_top
      g.find_const :Proc
      g.swap
      g.send :__from_block__, 1

      t.set!

      g.send_with_block :b, 0, false
    end
  end

  relates "a.b(4, &c)" do
    compile do |g|
      t = g.new_label

      g.push :self
      g.send :a, 0, true
      g.push 4
      g.push :self
      g.send :c, 0, true

      g.dup
      g.is_nil
      g.git t

      g.push_cpath_top
      g.find_const :Proc
      g.swap
      g.send :__from_block__, 1

      t.set!

      g.send_with_block :b, 1, false
    end
  end

  relates "a.b(1, 2, 3, &c)" do
    compile do |g|
      t = g.new_label

      g.push :self
      g.send :a, 0, true
      g.push 1
      g.push 2
      g.push 3
      g.push :self
      g.send :c, 0, true

      g.dup
      g.is_nil
      g.git t

      g.push_cpath_top
      g.find_const :Proc
      g.swap
      g.send :__from_block__, 1

      t.set!

      g.send_with_block :b, 3, false
    end
  end

  relates "a(&b)" do
    compile do |g|
      t = g.new_label

      g.push :self
      g.push :self
      g.send :b, 0, true

      g.dup
      g.is_nil
      g.git t

      g.push_cpath_top
      g.find_const :Proc
      g.swap
      g.send :__from_block__, 1

      t.set!

      g.send_with_block :a, 0, true
    end
  end

  relates "a(4, &b)" do
    compile do |g|
      t = g.new_label

      g.push :self
      g.push 4
      g.push :self
      g.send :b, 0, true

      g.dup
      g.is_nil
      g.git t

      g.push_cpath_top
      g.find_const :Proc
      g.swap
      g.send :__from_block__, 1

      t.set!

      g.send_with_block :a, 1, true
    end
  end

  relates "a(1, 2, 3, &b)" do
    compile do |g|
      t = g.new_label

      g.push :self
      g.push 1
      g.push 2
      g.push 3
      g.push :self
      g.send :b, 0, true

      g.dup
      g.is_nil
      g.git t

      g.push_cpath_top
      g.find_const :Proc
      g.swap
      g.send :__from_block__, 1

      t.set!

      g.send_with_block :a, 3, true
    end
  end

  relates "define_attr_method(:x, :sequence_name, &Proc.new { |*args| nil })" do
    compile do |g|
      t = g.new_label

      g.push :self
      g.push_literal :x
      g.push_literal :sequence_name

      g.push_const :Proc
      in_block_send :new, :splat, nil, 0, false do |d|
        d.push :nil
      end

      g.dup
      g.is_nil
      g.git t

      g.push_cpath_top
      g.find_const :Proc
      g.swap
      g.send :__from_block__, 1

      t.set!

      g.send_with_block :define_attr_method, 2, true
    end
  end

  relates "r.read_body(dest, &block)" do
    compile do |g|
      t = g.new_label

      g.push :self
      g.send :r, 0, true
      g.push :self
      g.send :dest, 0, true
      g.push :self
      g.send :block, 0, true

      g.dup
      g.is_nil
      g.git t

      g.push_cpath_top
      g.find_const :Proc
      g.swap
      g.send :__from_block__, 1

      t.set!

      g.send_with_block :read_body, 1, false
    end
  end

  relates "o.m(:a => 1, :b => 2)" do
    compile do |g|
      g.push :self
      g.send :o, 0, true
      g.push_cpath_top
      g.find_const :Hash
      g.push 2
      g.send :new_from_literal, 1

      g.dup
      g.push_literal :a
      g.push 1
      g.send :[]=, 2
      g.pop

      g.dup
      g.push_literal :b
      g.push 2
      g.send :[]=, 2
      g.pop

      g.send :m, 1, false
    end
  end

  relates "o.m(42, :a => 1, :b => 2)" do
    compile do |g|
      g.push :self
      g.send :o, 0, true
      g.push 42
      g.push_cpath_top
      g.find_const :Hash
      g.push 2
      g.send :new_from_literal, 1

      g.dup
      g.push_literal :a
      g.push 1
      g.send :[]=, 2
      g.pop

      g.dup
      g.push_literal :b
      g.push 2
      g.send :[]=, 2
      g.pop

      g.send :m, 2, false
    end
  end

  relates "o.m(42, :a => 1, :b => 2, *c)" do
    compile do |g|
      g.push :self
      g.send :o, 0, true
      g.push 42
      g.push_cpath_top
      g.find_const :Hash
      g.push 2
      g.send :new_from_literal, 1

      g.dup
      g.push_literal :a
      g.push 1
      g.send :[]=, 2
      g.pop

      g.dup
      g.push_literal :b
      g.push 2
      g.send :[]=, 2
      g.pop

      g.push :self
      g.send :c, 0, true
      g.cast_array
      g.push :nil
      g.send_with_splat :m, 2, false, false
    end
  end

  relates "a (1,2,3)" do
    compile do |g|
      g.push :self
      g.push 1
      g.push 2
      g.push 3
      g.send :a, 3, true
    end
  end

  relates "o.puts(42)" do
    compile do |g|
      g.push :self
      g.send :o, 0, true
      g.push 42
      g.send :puts, 1, false
    end
  end

  relates "1.b(c)" do
    compile do |g|
      g.push 1
      g.push :self
      g.send :c, 0, true
      g.send :b, 1, false
    end
  end

  relates "(v = (1 + 1)).zero?" do
    compile do |g|
      g.push 1
      g.push 1
      g.send :+, 1, false
      g.set_local 0
      g.send :zero?, 0, false
    end
  end

  relates "-2**31" do
    compile do |g|
      g.push 2
      g.push 31
      g.send :**, 1, false
      g.send :-@, 0, false
    end
  end

  relates "a[]" do
    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.send :[], 0, false
    end
  end

  relates "m(:a => 1, :b => 2)" do
    compile do |g|
      g.push :self
      g.push_cpath_top
      g.find_const :Hash
      g.push 2
      g.send :new_from_literal, 1

      g.dup
      g.push_literal :a
      g.push 1
      g.send :[]=, 2
      g.pop

      g.dup
      g.push_literal :b
      g.push 2
      g.send :[]=, 2
      g.pop

      g.send :m, 1, true
    end
  end

  relates "m(42, :a => 1, :b => 2)" do
    compile do |g|
      g.push :self
      g.push 42
      g.push_cpath_top
      g.find_const :Hash
      g.push 2
      g.send :new_from_literal, 1

      g.dup
      g.push_literal :a
      g.push 1
      g.send :[]=, 2
      g.pop

      g.dup
      g.push_literal :b
      g.push 2
      g.send :[]=, 2
      g.pop

      g.send :m, 2, true
    end
  end

  relates "m(42, :a => 1, :b => 2, *c)" do
    compile do |g|
      g.push :self
      g.push 42
      g.push_cpath_top
      g.find_const :Hash
      g.push 2
      g.send :new_from_literal, 1

      g.dup
      g.push_literal :a
      g.push 1
      g.send :[]=, 2
      g.pop

      g.dup
      g.push_literal :b
      g.push 2
      g.send :[]=, 2
      g.pop

      g.push :self
      g.send :c, 0, true
      g.cast_array
      g.push :nil

      g.send_with_splat :m, 2, true, false
    end
  end

  relates "m(42)" do
    compile do |g|
      g.push :self
      g.push 42
      g.send :m, 1, true
    end
  end

  relates "a(:b) { :c }" do
    compile do |g|
      g.push :self
      g.push_literal :b

      g.in_block_send :a, :none, nil, 1 do |d|
        d.push_literal :c
      end
    end
  end

  relates "a [42]" do
    compile do |g|
      g.push :self
      g.push 42
      g.make_array 1
      g.send :a, 1, true
    end
  end

  relates "42 if block_given?" do
    compile do |g|
      t = g.new_label
      f = g.new_label

      g.push :self
      g.send :block_given?, 0, true
      g.gif f
      g.push 42
      g.goto t
      f.set!
      g.push :nil
      t.set!
    end
  end

  relates "method" do
    compile do |g|
      g.push :self
      g.send :method, 0, true
    end
  end

  relates <<-ruby do
      a << begin
             b
           rescue
             c
           end
    ruby

    compile do |g|
      g.push :self
      g.send :a, 0, true

      for_rescue do |rb|
        rb.body do
          g.push :self
          g.send :b, 0, true
        end

        rb.condition :StandardError do
          g.push :self
          g.send :c, 0, true
        end
      end

      g.send :<<, 1, false
    end
  end

  relates "meth([*[1]])" do
    compile do |g|
      g.push :self
      g.push 1
      g.make_array 1
      g.send :meth, 1, true
    end
  end

  relates "meth(*[1])" do
    compile do |g|
      g.push :self

      g.push 1
      g.make_array 1
      g.cast_array

      g.push :nil

      g.send_with_splat :meth, 0, true, false
    end
  end
end
