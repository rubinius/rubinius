require File.dirname(__FILE__) + '/../spec_helper'

describe "An Alias node" do
  alias_both = lambda do |g|
    in_class :X do |d|
      d.push_scope
      d.push_literal :y
      d.push_literal :x
      d.send :alias_method, 2, true
    end
  end

  relates <<-ruby do
      class X
        alias :y :x
      end
    ruby

    parse do
      [:class, :X, nil, [:scope, [:alias, [:lit, :y], [:lit, :x]]]]
    end

    compile(&alias_both)
  end

  relates <<-ruby do
      class X
        alias y x
      end
    ruby

    parse do
      [:class, :X, nil, [:scope, [:alias, [:lit, :y], [:lit, :x]]]]
    end

    compile(&alias_both)
  end
end
