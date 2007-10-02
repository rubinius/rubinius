require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

@unboundmethod_to_s = shared "UnboundMethod#to_s" do |cmd|
  describe "UnboundMethod##{cmd}" do
    it "returns a string representing the UnboundMethod" do
      UnboundMethodSpecs::Methods.instance_method(:foo).send(cmd).should == "#<UnboundMethod: UnboundMethodSpecs::Methods#foo>"
      UnboundMethodSpecs::Methods.new.method(:foo).unbind.send(cmd).should == "#<UnboundMethod: UnboundMethodSpecs::Methods#foo>"
    end
  end
end

describe "UnboundMethod#to_s" do
  it_behaves_like(@unboundmethod_to_s, :to_s)
end
