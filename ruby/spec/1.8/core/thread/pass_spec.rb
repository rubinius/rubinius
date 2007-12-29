require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread.pass" do
  it "should at least be a noop returning nil" do
    Thread.pass.should == nil
  end
end
