require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is ""..."1.9" do

  require 'ftools'

  describe "File.makedirs" do
    before(:each) do
    end
    
    after(:each) do
      FileUtils.rm_rf("makedirs_test")
      flunk if File.exist?("makedirs_test")
    end
    
    it "creates the dirs from arg" do
      File.exist?("makedirs_test").should == false
      File.makedirs("makedirs_test/second_dir")
      File.exist?("makedirs_test").should == true
      File.directory?("makedirs_test").should == true
      File.exist?("makedirs_test/second_dir").should == true
      File.directory?("makedirs_test/second_dir").should == true
    end
  end
end
