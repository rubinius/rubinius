require File.dirname(__FILE__) + '/../../spec_helper'
require 'pathname'

describe "Pathname#parent" do

  it "should have parent of root as root" do
    Pathname.new('/').parent.to_s.should == '/'
  end

  it "should have parent of /usr/ as root" do
    Pathname.new('/usr/').parent.to_s.should == '/'
  end

  it "should have parent of /usr/local as root" do
    Pathname.new('/usr/local').parent.to_s.should == '/usr'
  end

end

