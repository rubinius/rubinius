require File.dirname(__FILE__) + '/../../spec_helper'
require 'etc'

platform_is :windows do 
  describe "Etc.getpwuid" do
    it "returns nil" do
      Etc.getpwuid(1).should == nil
      Etc.getpwuid(nil).should == nil
      Etc.getpwuid('nil').should == nil
    end
  end
end

platform_is_not :windows do
  describe "Etc.getpwuid" do
    it "returns a Passwd struct instance for the given user" do
      pw = Etc.getpwuid(`id -u`.strip.to_i)

      deviates_on :rubinius do
        pw.is_a?(Etc::Passwd).should == true
      end

      compliant_on(:ruby, :jruby) do
        pw.is_a?(Struct::Passwd).should == true
      end
    end

    it "only accepts integers as argument" do
      lambda {
        Etc.getpwuid("foo")
        Etc.getpwuid(nil)
      }.should raise_error(TypeError)
    end

    deviates_on :rubinius, :ruby19 do
      it "uses Process.uid as the default value for the argument" do
        pw = Etc.getpwuid
      end
    end
  end
end
