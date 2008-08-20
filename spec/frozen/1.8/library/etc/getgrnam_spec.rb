require File.dirname(__FILE__) + '/../../spec_helper'
require 'etc'

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
