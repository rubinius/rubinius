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

    # sclass
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

    # sclass trailing class
  end
end
