require File.dirname(__FILE__) + '/../../spec_helper'

describe "Symbol.all_symbols" do
  it "returns an array containing all the Symbols in the symbol table" do
    Symbol.all_symbols.is_a?(Array).should == true
    Symbol.all_symbols.all? { |s| s.is_a?(Symbol) ? true : (p s; false) }.should == true
  end
  # it "is a pain in the ass to test..."
end
