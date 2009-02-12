require File.dirname(__FILE__) + '/../../spec_helper'
require 'syslog'

describe "Syslog.mask" do
  platform_is_not :windows do

    before :each do
      Syslog.opened?.should be_false
    end

    after :each do
      Syslog.opened?.should be_false
    end

    # make sure we return the mask to the default value
    after :all do
      Syslog.open { |s| s.mask = 255 }
    end

    it "returns the log priority mask" do
      Syslog.open("rubyspec")
      Syslog.mask.should == 255
      Syslog.mask = 3
      Syslog.mask.should == 3
      Syslog.mask = 255
      Syslog.close
    end

    it "defaults to 255" do
      Syslog.open do |s|
        s.mask.should == 255
      end
    end

    it "returns nil if the log is closed" do
      Syslog.opened?.should == false
      Syslog.mask.should == nil
    end

    it "persists if the log is reopened" do
      Syslog.open
      Syslog.mask.should == 255
      Syslog.mask = 64

      Syslog.reopen("rubyspec")
      Syslog.mask.should == 64
      Syslog.close

      Syslog.open
      Syslog.mask.should == 64
      Syslog.close
    end
  end
end

describe "Syslog.mask=" do
  platform_is_not :windows do

    before :each do
      Syslog.opened?.should be_false
    end

    after :each do
      Syslog.opened?.should be_false
    end

    # make sure we return the mask to the default value
    after :all do
      Syslog.open { |s| s.mask = 255 }
    end

    it "sets the log priority mask" do
      Syslog.open
      Syslog.mask = 64
      Syslog.mask.should == 64
      Syslog.close
    end
    
    it "raises an error if the log is closed" do
      lambda { Syslog.mask = 1337 }.should raise_error(RuntimeError)
    end

    it "only accepts numbers" do
      Syslog.open

      Syslog.mask = 1337
      Syslog.mask.should == 1337

      Syslog.mask = 3.1416
      Syslog.mask.should == 3

      lambda { Syslog.mask = "oh hai" }.should raise_error(TypeError)
      lambda { Syslog.mask = "43" }.should raise_error(TypeError)

      Syslog.close
    end
  end
end
