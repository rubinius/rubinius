require File.dirname(__FILE__) + '/../spec_helper'

describe "An Sclass node" do
  relates <<-ruby do
      class << self
        42
      end
    ruby

    parse do
      [:sclass, [:self], [:scope, [:lit, 42]]]
    end

    compile do |g|
      g.push :self
      g.dup
      g.send :__verify_metaclass__, 0 # TODO: maybe refactor...
      g.pop
      g.push_const :Rubinius
      g.swap
      g.send :open_metaclass, 1
      g.dup

      g.push_const :Rubinius
      g.swap
      g.push_literal :__metaclass_init__
      g.swap
      g.push_literal_desc do |d2|
        d2.push_self
        d2.add_scope
        d2.push 42
        d2.ret
      end
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

    parse do
      [:class,
       :A,
       nil,
       [:scope,
        [:block,
         [:sclass, [:self], [:scope, [:call, nil, :a, [:arglist]]]],
         [:class, :B, nil, [:scope]]]]]
    end

    compile do |g|
      in_class :A do |d|
        d.push :self
        d.dup
        d.send :__verify_metaclass__, 0 # TODO: maybe refactor...
        d.pop
        d.push_const :Rubinius
        d.swap
        d.send :open_metaclass, 1
        d.dup

        d.push_const :Rubinius
        d.swap
        d.push_literal :__metaclass_init__
        d.swap

        d.push_literal_desc do |d2|
          d2.push_self
          d2.add_scope
          d2.push :self
          d2.send :a, 0, true
          d2.ret
        end
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

    parse do
      [:block,
        [:lasgn, :x, [:str, "a"]],
        [:sclass, [:lvar, :x], [:scope]]]
    end

    compile do |g|
      g.push_literal "a"
      g.string_dup
      g.set_local 0
      g.pop
      g.push_local 0
      g.dup
      g.send :__verify_metaclass__, 0
      g.pop
      g.push_const :Rubinius
      g.swap
      g.send :open_metaclass, 1
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
        d.dup
        d.send :__verify_metaclass__, 0
        d.pop
        d.push_const :Rubinius
        d.swap
        d.send :open_metaclass, 1
        d.pop
        d.push :nil
      end
    end
  end
end
