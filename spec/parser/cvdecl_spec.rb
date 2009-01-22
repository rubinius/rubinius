require File.dirname(__FILE__) + '/../spec_helper'

describe "A Cvdecl node" do
  relates <<-ruby do
      class X
        @@blah = 1
      end
    ruby

    parse do
      [:class, :X, nil, [:scope, [:cvdecl, :@@blah, [:lit, 1]]]]
    end
  end
end
