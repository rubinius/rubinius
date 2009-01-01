require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO" do
  it "includes File::Constants" do
    IO.include?(File::Constants).should == true
  end

  it "for_fd takes two arguments" do
	IO.method(:for_fd).arity.should == -1
  end
end
