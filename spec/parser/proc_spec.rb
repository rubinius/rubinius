require File.dirname(__FILE__) + '/../spec_helper'

describe "A Proc node" do
  relates "proc { || (x + 1) }" do
    parse do
      [:iter,
       [:call, nil, :proc, [:arglist]],
       0,
       [:call, [:call, nil, :x, [:arglist]], :+, [:arglist, [:lit, 1]]]]
    end

    # proc args 0
  end

  relates "proc { |x| (x + 1) }" do
    parse do
      [:iter,
       [:call, nil, :proc, [:arglist]],
       [:lasgn, :x],
       [:call, [:lvar, :x], :+, [:arglist, [:lit, 1]]]]
    end

    # proc args 1
  end

  relates "proc { |x, y| (x + y) }" do
    parse do
      [:iter,
       [:call, nil, :proc, [:arglist]],
       [:masgn, [:array, [:lasgn, :x], [:lasgn, :y]]],
       [:call, [:lvar, :x], :+, [:arglist, [:lvar, :y]]]]
    end

    # proc args 2
  end

  relates "proc { (x + 1) }" do
    parse do
      [:iter,
       [:call, nil, :proc, [:arglist]],
       nil,
       [:call, [:call, nil, :x, [:arglist]], :+, [:arglist, [:lit, 1]]]]
    end

    # proc args no
  end
end
