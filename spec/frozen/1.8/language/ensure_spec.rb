require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/fixtures/ensure'

describe "The ensure keyword" do
  it "executes as a result of a throw within it's block" do
    i = []
    catch(:exit) do
      begin
        i << :begin
        throw :exit
      ensure
        i << :ensure
      end
      i << :after_ensure
    end    
    i.should == [:begin,:ensure]
  end

  it "is run when calling a block that contains a return" do
    t = EnsureSpec::Test.new
    t.do_test.should == :did_test
    t.values.should == [:start, :in_block, :end]
  end
  
  it "propagates the original exception even when exceptions are caught inside the ensure block" do
    code = lambda do
      begin
        raise EOFError, "foo"
      ensure
        "".size rescue nil
      end
    end
    code.should raise_error(EOFError)
  end
end
