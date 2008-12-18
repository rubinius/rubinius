require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/expectations/expectations'
require 'mspec/helpers/flunk'
require 'mspec/runner/mspec'

describe Object, "#flunk" do
  before :each do
    MSpec.stub!(:actions)
    MSpec.stub!(:current).and_return(mock("spec state", :null_object => true))
  end

  it "raises an ExpectationNotMetError unconditionally" do
    lambda { flunk }.should raise_error(ExpectationNotMetError)
  end

  it "accepts on argument for an optional message" do
    lambda {flunk "test"}.should raise_error(ExpectationNotMetError)
  end
end
