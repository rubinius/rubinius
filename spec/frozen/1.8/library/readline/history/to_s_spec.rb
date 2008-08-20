require File.dirname(__FILE__) + '/../../../spec_helper'

has_tty? do # needed for CI until we figure out a better way
require 'readline'

describe "Readline::HISTORY.to_s" do
  it "returns 'HISTORY'" do
    Readline::HISTORY.to_s.should == "HISTORY"
  end
end
end
