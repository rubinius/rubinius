require File.dirname(__FILE__) + '/../../spec_helper'
require 'etc'

platform_is :windows do
  describe "Etc.getgrnam" do
    it "returns nil" do
      Etc.getgrnam(1).should == nil
      Etc.getgrnam(nil).should == nil
      Etc.getgrnam('nil').should == nil
    end
  end
end

platform_is_not :windows do
  describe "Etc.getgrnam" do
    it "returns a Group struct instance for the given group" do
      gr = Etc.getgrnam("daemon")

      deviates_on :rubinius do
        gr.is_a?(Etc::Group).should == true
      end

      compliant_on(:ruby, :jruby) do
        gr.is_a?(Struct::Group).should == true
      end
    end

    it "only accepts strings as argument" do
      lambda {
        Etc.getgrnam(123)
        Etc.getgrnam(nil)
      }.should raise_error(TypeError)
    end
  end
end
