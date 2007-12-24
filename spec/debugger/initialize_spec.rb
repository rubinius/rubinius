require File.dirname(__FILE__) + "/spec_helper"

describe "Debugger.instance" do
  it "retrieves the singleton instance" do
    dbg = Debugger.instance
    dbg.should_not == nil
    dbg2 = Debugger.instance
    dbg.should == dbg2
  end
end
