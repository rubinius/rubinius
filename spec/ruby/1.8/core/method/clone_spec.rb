require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Method#clone" do
  it "returns a copy of the method" do
    m1 = MethodSpecs::Methods.new.method(:foo)
    m2 = m1.clone

    (m1 == m2).should == true
    m1.eql?(m2).should == false

    m1.call.should == m2.call
  end
end
