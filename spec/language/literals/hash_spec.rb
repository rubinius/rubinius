require File.dirname(__FILE__) + '/../../spec_helper'

context "Hash literal" do
  specify "{} should return a new hash populated with the given elements" do
    example do
      {:a => 'a', 'b' => 3, 44 => 2.3}
    end.should == {:a => "a", "b" => 3, 44 => 2.3}
  end
end
