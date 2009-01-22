require File.dirname(__FILE__) + '/../spec_helper'

describe "A Hash node" do
  relates "{ 1 => 2, 3 => 4 }" do
    parse do
      [:hash, [:lit, 1], [:lit, 2], [:lit, 3], [:lit, 4]]
    end

    # hash
  end

  relates "{ 1 => (2 rescue 3) }" do
    parse do
      [:hash, [:lit, 1], [:rescue, [:lit, 2], [:resbody, [:array], [:lit, 3]]]]
    end

    # hash rescue
  end
end
