require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#initialize" do
  it "sets the constant with the given name to the given value" do
    m = Module.new do
      const_set :A, "A"
    end
    m.const_get("A").should == "A"
  end
end