require File.expand_path('../../spec_helper', __FILE__)

describe "An Sclass node" do
  relates <<-ruby do
      class << self
        42
      end
    ruby

    compile do |g|
      g.push :self
      g.push_type
      g.swap
      g.send :object_singleton_class, 1

      d = new_generator(g)

      g.create_block d

      d.push_self
      d.add_scope
      d.push 42
      d.ret

      g.swap
      g.push_scope
      g.push_true
      g.send :call_under, 3
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
        d.push_type
        d.swap
        d.send :object_singleton_class, 1

        e = new_generator(g)

        d.create_block(e)

        e.push_self
        e.add_scope
        e.push :self
        e.send :a, 0, true
        e.ret

        d.swap
        d.push_scope
        d.push_true
        d.send :call_under, 3

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
      g.push_type
      g.swap
      g.send :object_singleton_class, 1
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
        d.push_type
        d.swap
        d.send :object_singleton_class, 1
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
      g.push_type
      g.swap
      g.send :object_singleton_class, 1
    end
  end
end
