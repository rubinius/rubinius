require File.dirname(__FILE__) + '/../../spec_helper'
require 'etc'

describe "Etc.getpwnam" do
  it "returns a Passwd struct instance for the given user" do
    pw = Etc.getpwnam(`whoami`.strip)

    deviates_on :rubinius do
      pw.is_a?(Etc::Passwd).should == true
    end

    compliant_on(:ruby, :jruby) do
      pw.is_a?(Struct::Passwd).should == true
    end
  end

  it "only accepts strings as argument" do
    lambda {
      Etc.getpwnam(123)
      Etc.getpwnam(nil)
    }.should raise_error(TypeError)
  end
end
