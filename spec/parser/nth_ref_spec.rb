require File.dirname(__FILE__) + '/../spec_helper'

describe "An Nth_ref node" do
  relates "$1" do
    parse do
      [:nth_ref, 1]
    end

    # nth ref
  end
end
