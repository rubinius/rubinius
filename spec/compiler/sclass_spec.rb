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

      g.push_literal_desc do |d2|
        d2.push_self
        d2.add_scope
        d2.push 42
        d2.ret
      end

      g.swap
      g.attach_method :__metaclass_init__
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

        d.push_literal_desc do |d2|
          d2.push_self
          d2.add_scope
          d2.push :self
          d2.send :a, 0, true
          d2.ret
        end

        d.swap
        d.attach_method :__metaclass_init__
        d.pop
        d.send :__metaclass_init__, 0
        d.pop
        d.in_class :B
        d.pop
        d.push :nil
      end
    end
  end
end
