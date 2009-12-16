require File.dirname(__FILE__) + '/../../spec_helper'
require 'syslog'

describe "Syslog.instance" do
  platform_is_not :windows do
    it "returns the module" do
      Syslog.instance.should == Syslog
    end
  end
end
