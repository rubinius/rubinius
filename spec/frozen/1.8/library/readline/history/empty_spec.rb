require File.dirname(__FILE__) + '/../../../spec_helper'

has_tty? do # needed for CI until we figure out a better way
require 'readline'

describe "Readline::HISTORY.empty?" do
  it "returns true when the history is empty" do
    Readline::HISTORY.should be_empty
    Readline::HISTORY.push("test")
    Readline::HISTORY.should_not be_empty
    Readline::HISTORY.pop
    Readline::HISTORY.should be_empty
  end
end
end
