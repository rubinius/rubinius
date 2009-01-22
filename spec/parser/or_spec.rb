require File.dirname(__FILE__) + '/../spec_helper'

describe "An Or node" do
  relates "(a or b)" do
    parse do
      [:or, [:call, nil, :a, [:arglist]], [:call, nil, :b, [:arglist]]]
    end

    # or
  end

  relates "((a || b) || (c && d))" do
    parse do
      [:or,
       [:or, [:call, nil, :a, [:arglist]], [:call, nil, :b, [:arglist]]],
       [:and, [:call, nil, :c, [:arglist]], [:call, nil, :d, [:arglist]]]]
    end

    # or big2
  end

  relates "((a or b) or (c and d))" do
    parse do
      [:or,
       [:or, [:call, nil, :a, [:arglist]], [:call, nil, :b, [:arglist]]],
       [:and, [:call, nil, :c, [:arglist]], [:call, nil, :d, [:arglist]]]]
    end

    # or big
  end
end
