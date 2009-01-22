require File.dirname(__FILE__) + '/../spec_helper'

describe "A Dsym node" do
  relates ':"x#{(1 + 1)}y"' do
    parse do
      [:dsym,
       "x",
       [:evstr, [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]]],
       [:str, "y"]]
    end

    # dsym
  end
end
