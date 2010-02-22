require File.expand_path('../../../spec_helper', __FILE__)

describe "Float#==" do
  it "returns true if self has the same value as other" do
    (1.0 == 1).should == true
    (2.71828 == 1.428).should == false
    (-4.2 == 4.2).should == false
  end

  it "calls 'other == self' if coercion fails" do
    class X; def ==(other); 2.0 == other; end; end

    (1.0 == X.new).should == false
    (2.0 == X.new).should == true
  end
end
