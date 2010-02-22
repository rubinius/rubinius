require File.expand_path('../../../spec_helper', __FILE__)
require 'etc'

describe "Etc.getlogin" do
  it "returns the name of the user who runs this process" do
    Etc.getlogin.should == username
  end
end
