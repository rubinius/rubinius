require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/tmp'

describe Object, "#tmp" do
  it "returns a name relative to the current working directory" do
    dir = "#{File.expand_path(Dir.pwd)}/rubyspec_temp"

    tmp("test.txt").should == dir + "/test.txt"
  end
end
