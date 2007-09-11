require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#to_sym" do
  it "returns the symbol corresponding to self" do
    "Koala".to_sym.should == :Koala
    'cat'.to_sym.should == :cat
    '@cat'.to_sym.should == :@cat
    
    'cat and dog'.to_sym.should == :"cat and dog"
  end
  
  it "raises an ArgumentError when self can't be converted to symbol" do
    should_raise(ArgumentError) { "".to_sym }
    should_raise(ArgumentError) { "foo\x00bar".to_sym }
  end
end
