require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#fileno" do
  it "is nil" do
    @io = StringIO.new("nuffin")
    @io.fileno.should == nil
  end
end
