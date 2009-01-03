require File.dirname(__FILE__) + '/../../spec_helper'
require 'etc'

describe "Etc.getlogin" do
  it "returns the name of the user who runs this process" do
    Etc.getlogin.should == username
  end
end
