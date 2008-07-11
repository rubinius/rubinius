require File.dirname(__FILE__) + '/../../spec_helper'

has_tty? do # needed for CI until we figure out a better way
require 'readline'

# Note: additional specs for HISTORY are in 'history' subdir. 
describe "Readline::HISTORY" do
  it "is defined" do
    Readline.const_defined?(:HISTORY).should == true
  end
end

describe "Readline::VERSION" do
  it "is defined and is a non-empty String" do
    Readline.const_defined?(:VERSION).should == true
    Readline::VERSION.should be_kind_of(String)
    Readline::VERSION.should_not be_empty
  end
end
end
