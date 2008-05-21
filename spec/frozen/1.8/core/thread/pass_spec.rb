require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread.pass" do
  it "returns nil" do
    Thread.pass.should == nil
  end
end
