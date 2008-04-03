require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../expectations'
require File.dirname(__FILE__) + '/../../expectations/flunk'

describe Object, "#flunk" do
  before :each do
    MSpec.stub!(:actions)
    MSpec.stub!(:current).and_return(mock("spec state", :null_object => true))
  end

  it "raises an ExpectationNotMetError unconditionally" do
    lambda { flunk }.should raise_error(ExpectationNotMetError)
  end
end
