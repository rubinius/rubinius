require File.dirname(__FILE__) + '/../spec_helper'

describe "An Nth_ref node" do
  relates "$1" do
    parse do
      [:nth_ref, 1]
    end

    compile do |g|
      g.push_variables
      g.push 1
      g.send :nth_ref, 1
    end
  end
end
