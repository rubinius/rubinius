require File.dirname(__FILE__) + '/../../../spec_helper'
require 'readline'

describe "Readline::HISTORY.to_s" do
  it "returns 'HISTORY'" do
    Readline::HISTORY.to_s.should == "HISTORY"
  end
end
