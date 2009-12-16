require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.8.7" do
  describe "GC.stress" do
    after :each do
      # make sure that we never leave these tests in stress enabled GC!
      GC.stress = false
    end
    it "returns current status of GC stress mode" do
      GC.stress.should == false
      GC.stress = true
      GC.stress.should == true
      GC.stress = false
      GC.stress.should == false
    end
  end

  describe "GC.stress=" do
    after :each do
      GC.stress = false
    end
    it "can be invoked without any exceptions" do
      lambda { GC.stress = true }.should_not raise_error
      lambda { GC.stress = false }.should_not raise_error
    end
    it "returns a proper boolean result" do
      GC.send(:stress=, true).should be_true
      GC.send(:stress=, false).should be_false
    end

  end
end
