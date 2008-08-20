require File.dirname(__FILE__) + '/../../../spec_helper'

has_tty? do # needed for CI until we figure out a better way
require 'readline'

describe "Readline::HISTORY" do
  it "is extended with the Enumerable module" do
    Readline::HISTORY.should be_kind_of(Enumerable)
  end
end
end
