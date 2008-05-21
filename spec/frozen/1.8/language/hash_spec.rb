require File.dirname(__FILE__) + '/../spec_helper'

describe "Hash literal" do
  it "{} should return an empty hash" do
    {}.size.should == 0
    {}.should == {}
  end

  it "{} should return a new hash populated with the given elements" do
    h = {:a => 'a', 'b' => 3, 44 => 2.3}
    h.size.should == 3
    h.should == {:a => "a", "b" => 3, 44 => 2.3}
  end
end
