require File.dirname(__FILE__) + '/../../spec_helper'

@integer_to_i = shared "Integer#to_i" do |cmd|
  describe "Integer##{cmd}" do
    it "returns self" do
      a = 1
      a.send(cmd).should == 1 
      a.send(cmd).eql?(a).should == true
    end
  end
end

describe "Integer#to_i" do
  it_behaves_like(@integer_to_i, :to_i)
end
