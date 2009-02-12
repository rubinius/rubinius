require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

ruby_version_is '' ... '1.9' do
  describe "Thread#critical" do
    it "returns the current critical state" do
      Thread.critical.should == false
    end
  end

  describe "Thread#critical=" do
    it "sets the current critical state" do
      Thread.critical.should == false
      begin
        Thread.critical = true
        Thread.critical.should == true
      ensure
        Thread.critical = false
      end
    end
  end
end
