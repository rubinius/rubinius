require File.dirname(__FILE__) + '/../../spec_helper'
require 'pathname'

describe "Pathname#absolute?" do

  it "should return true for the root directory" do
    Pathname.new('/').absolute?.should == true
  end

  it "should return true for a dir starting with a slash" do
    Pathname.new('/usr/local/bin').absolute?.should == true
  end

  it "should return false for a dir not starting with a slash" do
    Pathname.new('fish').absolute?.should == false
  end

  it "should return false for a dir not starting with a slash" do
    Pathname.new('fish/dog/cow').absolute?.should == false
  end

end

