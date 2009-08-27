require File.dirname(__FILE__) + '/../spec_helper'

describe "A Cvar node" do
  relates "@@x" do
    parse do
      [:cvar, :@@x]
    end

    compile do |g|
      g.push_scope
      g.push_literal :@@x
      g.send :class_variable_get, 1
    end
  end

  relates <<-ruby do
      class A
        @@a
      end
    ruby

    parse do
      [:class, :A, nil, [:scope, [:cvar, :@@a]]]
    end

    compile do |g|
      in_class :A do |d|
        d.push :self
        d.push_literal :@@a
        d.send :class_variable_get, 1
      end
    end
  end

  relates <<-ruby do
      module M
        @@a
      end
    ruby

    parse do
      [:module, :M, [:scope, [:cvar, :@@a]]]
    end

    compile do |g|
      in_module :M do |d|
        d.push :self
        d.push_literal :@@a
        d.send :class_variable_get, 1
      end
    end
  end
end
