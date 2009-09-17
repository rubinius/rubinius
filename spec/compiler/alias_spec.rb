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

  relates <<-ruby do
      class X
        alias :"y\#{1}" :"x\#{2}"
      end
    ruby

    parse do
      [:class,
       :X,
       nil,
       [:scope,
        [:alias,
         [:dsym, "y", [:evstr, [:lit, 1]]],
         [:dsym, "x", [:evstr, [:lit, 2]]]]]]
    end

    compile do |g|
      in_class :X do |d|
        d.push_scope

        d.push 1
        d.send :to_s, 0, true
        d.push_literal "y"
        d.string_dup
        d.string_append
        d.send :to_sym, 0, true

        d.push 2
        d.send :to_s, 0, true
        d.push_literal "x"
        d.string_dup
        d.string_append
        d.send :to_sym, 0, true

        d.send :alias_method, 2, true
      end
    end
  end
end
