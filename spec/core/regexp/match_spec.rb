require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/match'

describe "Regexp#=~" do
  it_behaves_like(:regexp_match, :=~)
end

describe "Regexp#match" do
  it_behaves_like(:regexp_match, :match)
end

describe "Regexp#~" do
  it "matchs against the contents of $_" do
    $_ = "input data"
    (~ /at/).should == 7
  end
end
