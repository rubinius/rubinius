require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO" do
  it "includes File::Constants" do
    IO.include?(File::Constants).should == true
  end
end
