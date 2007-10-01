require File.dirname(__FILE__) + '/../../spec_helper'

@method_to_s = shared "Method#to_s" do |cmd|
  describe "Method##{cmd}" do
    it "returns a string representing the method" do
      11.method("+").send(cmd).should == "#<Method: Fixnum#+>"
    end
  end
end

describe "Method#to_s" do
  it_behaves_like(@method_to_s, :to_s)
end
