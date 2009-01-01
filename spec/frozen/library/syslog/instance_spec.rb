require File.dirname(__FILE__) + '/../../spec_helper'
require 'syslog'

describe "Syslog.instance" do
  not_supported_on :windows do
    it "returns the module" do
      Syslog.instance.should == Syslog
    end
  end
end
