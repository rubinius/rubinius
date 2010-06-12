require File.expand_path('../../spec_helper', __FILE__)

describe "A Super node" do
  relates <<-ruby do
      def x
        super()
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.push_block
        d.send_super :x, 0
      end
    end
  end

  relates <<-ruby do
      def x(&block)
        super(&block)
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.block_arg 0

        is_nil = d.new_label
        d.push_local 0
        d.dup
        d.is_nil
        d.git is_nil
        d.push_cpath_top
        d.find_const :Proc
        d.swap
        d.send :__from_block__, 1
        is_nil.set!
        d.send_super :x, 0
      end
    end
  end

  relates <<-ruby do
      def x
        super([24, 42])
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.push 24
        d.push 42
        d.make_array 2
        d.push_block
        d.send_super :x, 1
      end
    end
  end

  relates <<-ruby do
      def x
        super(4)
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.push 4
        d.push_block
        d.send_super :x, 1
      end
    end
  end

  relates "super(a, &b)" do
    compile do |g|
      t = g.new_label

      g.push :self
      g.send :a, 0, true

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

      # nil here because the test isn't wrapped in a method, so
      # there is no current method to pull the name of the method
      # from
      g.send_super nil, 1
    end
  end

  relates "super(a, *b)" do
    compile do |g|
      g.push :self
      g.send :a, 0, true

      g.push :self
      g.send :b, 0, true

      g.cast_array

      g.push_block

      # nil here because the test isn't wrapped in a method, so
      # there is no current method to pull the name of the method
      # from
      g.send_super nil, 1, true
    end
  end

  relates <<-ruby do
      def f(*)
        super
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.push_block
        d.zsuper :f
      end
    end
  end

  relates <<-ruby do
      def x
        super(24, 42)
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.push 24
        d.push 42
        d.push_block
        d.send_super :x, 2
      end
    end
  end

  relates "super([*[1]])" do
    compile do |g|
      g.push 1
      g.make_array 1
      g.push_block
      g.send_super nil, 1
    end
  end

  relates "super(*[1])" do
    compile do |g|
      g.push 1
      g.make_array 1
      g.cast_array

      g.push_block
      g.send_super nil, 0, true
    end
  end
end
