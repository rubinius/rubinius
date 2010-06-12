require File.expand_path('../../spec_helper', __FILE__)

describe "An Sclass node" do
  relates <<-ruby do
      class << self
        42
      end
    ruby

    compile do |g|
      g.push :self
      g.push_rubinius
      g.swap
      g.send :object_metaclass, 1
      g.dup

      g.push_rubinius
      g.swap
      g.push_literal :__metaclass_init__
      g.swap

      d = new_generator(g)

      d.push_self
      d.add_scope
      d.push 42
      d.ret

      g.push_literal(d)

      g.swap
      g.push_scope
      g.swap
      g.send :attach_method, 4

      g.pop
      g.send :__metaclass_init__, 0
    end
  end

  relates <<-ruby do
      class A
        class << self
          a
        end
        class B
        end
      end
    ruby

    compile do |g|
      in_class :A do |d|
        d.push :self
        d.push_rubinius
        d.swap
        d.send :object_metaclass, 1
        d.dup

        d.push_rubinius
        d.swap
        d.push_literal :__metaclass_init__
        d.swap

        e = new_generator(g)

        e.push_self
        e.add_scope
        e.push :self
        e.send :a, 0, true
        e.ret

        d.push_literal(e)

        d.swap
        d.push_scope
        d.swap
        d.send :attach_method, 4

        d.pop
        d.send :__metaclass_init__, 0
        d.pop
        d.in_class :B
        d.pop
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      x = "a"
      class << x
      end
    ruby

    compile do |g|
      g.push_literal "a"
      g.string_dup
      g.set_local 0
      g.pop
      g.push_local 0
      g.push_rubinius
      g.swap
      g.send :object_metaclass, 1
      g.pop
      g.push :nil
    end
  end

  relates <<-ruby do
    x = "a"
    m do
      class << x
      end
    end
    ruby

    compile do |g|
      g.push_literal "a"
      g.string_dup
      g.set_local 0
      g.pop

      g.push :self

      g.in_block_send :m, :none do |d|
        d.push_local_depth 1, 0
        d.push_rubinius
        d.swap
        d.send :object_metaclass, 1
        d.pop
        d.push :nil
      end
    end
  end

  relates <<-ruby do
    x = "a"
    class << x
      self
    end
    ruby

    compile do |g|
      g.push_literal "a"
      g.string_dup
      g.set_local 0
      g.pop

      g.push_local 0
      g.push_rubinius
      g.swap
      g.send :object_metaclass, 1
    end
  end
end
