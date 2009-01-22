require File.dirname(__FILE__) + '/../spec_helper'

describe "A Yield node" do
  relates "yield(42, 24)" do
    parse do
      [:yield, [:lit, 42], [:lit, 24]]
    end

    # yield n
  end

  relates "yield([42, 24])" do
    parse do
      [:yield, [:array, [:lit, 42], [:lit, 24]]]
    end

    # yield array n
  end

  relates "yield([42])" do
    parse do
      [:yield, [:array, [:lit, 42]]]
    end

    # yield array 1
  end

  relates "yield([])" do
    parse do
      [:yield, [:array]]
    end

    # yield array 0
  end

  relates "yield(42)" do
    parse do
      [:yield, [:lit, 42]]
    end

    # yield 1
  end

  relates "yield" do
    parse do
      [:yield]
    end

    # yield 0
  end

  relates "yield([*[1]])" do
    parse do
      [:yield, [:array, [:splat, [:array, [:lit, 1]]]]]
    end

    # splat yield array
  end

  relates "yield(*[1])" do
    parse do
      [:yield, [:splat, [:array, [:lit, 1]]]]
    end

    # splat yield
  end
end
