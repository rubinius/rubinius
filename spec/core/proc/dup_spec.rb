require File.dirname(__FILE__) + '/../../spec_helper'

@proc_dup = shared "Proc#dup" do |cmd|
  describe "Proc##{cmd}" do
    it "returns a copy of self" do
      a = lambda { "hello" }
      b = a.send(cmd)
      
      a.should == b
      a.equal?(b).should == false
      
      a.call.should == b.call
    end
  end
end

describe "Proc#dup" do
  it_behaves_like(@proc_dup, :dup)
end