require File.dirname(__FILE__) + '/../spec_helper'

describe "A Flip3 node" do
  relates <<-ruby do
      x = if ((i % 4) == 0)...((i % 3) == 0) then
        i
      else
        nil
      end
    ruby

    parse do
      [:lasgn,
       :x,
       [:if,
        [:flip3,
         [:call,
          [:call, [:call, nil, :i, [:arglist]], :%, [:arglist, [:lit, 4]]],
          :==,
          [:arglist, [:lit, 0]]],
         [:call,
          [:call, [:call, nil, :i, [:arglist]], :%, [:arglist, [:lit, 3]]],
          :==,
          [:arglist, [:lit, 0]]]],
        [:call, nil, :i, [:arglist]],
        [:nil]]]
    end

    # flip3
  end
end
