require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Method#to_proc" do
  before(:each) do
    @m = MethodSpecs::Methods.new
    @meth = @m.method(:foo)
  end

  it "returns a Proc object corresponding to the method" do
    @meth.to_proc.kind_of?(Proc).should == true
  end

  it "Proc object should have the correct arity" do
    # This may seem redundant but this bug has cropped up in jruby, mri and yarv.
    # http://jira.codehaus.org/browse/JRUBY-124
    @m.method(:zero).to_proc.arity.should == 0
    @m.method(:one).to_proc.arity.should == 1
    @m.method(:two).to_proc.arity.should == 2
    @m.method(:neg_one).to_proc.arity.should == -1
    @m.method(:neg_two).to_proc.arity.should == -2
    @m.method(:neg_three).to_proc.arity.should == -3
  end    
end
