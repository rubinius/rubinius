require File.dirname(__FILE__) + '/../../spec_helper'
require 'pathname'

describe "Pathname#relative?" do

  it "should return false for the root directory" do
    Pathname.new('/').relative?.should == false
  end

  it "should return false for a dir starting with a slash" do
    Pathname.new('/usr/local/bin').relative?.should == false
  end

  it "should return true for a dir not starting with a slash" do
    Pathname.new('fish').relative?.should == true
  end

  it "should return true for a dir not starting with a slash" do
    Pathname.new('fish/dog/cow').relative?.should == true
  end

end

