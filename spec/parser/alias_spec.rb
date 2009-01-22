require File.dirname(__FILE__) + '/../spec_helper'

describe "An Alias node" do
  relates <<-ruby do
      class X
        alias :y :x
      end
    ruby

    parse do
      [:class, :X, nil, [:scope, [:alias, [:lit, :y], [:lit, :x]]]]
    end

    # alias
  end

  relates <<-ruby do
      class X
        alias y x
      end
    ruby

    parse do
      [:class, :X, nil, [:scope, [:alias, [:lit, :y], [:lit, :x]]]]
    end

    #alias_ugh
  end
end
