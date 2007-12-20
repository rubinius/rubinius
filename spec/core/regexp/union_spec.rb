require File.dirname(__FILE__) + '/../../spec_helper'

describe "Regexp.union" do
  it "returns /(?!)/ when passed no arguments" do
    Regexp.union.should == /(?!)/
  end
  
  it "returns a regular expression that will match passed arguments" do
    Regexp.union("penzance").should == /penzance/
    Regexp.union("skiing", "sledding").should == /skiing|sledding/
    Regexp.union(/dogs/, /cats/i).should == /(?-mix:dogs)|(?i-mx:cats)/
  end
end
