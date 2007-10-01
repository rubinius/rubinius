require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Method#==" do
  before(:each) do
    @m = MethodSpecs::Methods.new
  end

  it "returns true if methods are the same" do
    m1 = @m.method(:foo)
    m2 = @m.method(:foo)

    (m1 == m1).should == true
    (m1 == m2).should == true
  end

  it "returns true on aliased methods" do
    m1 = @m.method(:foo)
    m2 = @m.method(:bar)

    (m1 == m2).should == true
  end
end
