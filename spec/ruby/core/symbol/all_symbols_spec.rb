require File.expand_path('../../../spec_helper', __FILE__)

describe "Symbol.all_symbols" do
  it "returns an Array" do
    Symbol.all_symbols.should be_an_instance_of(Array)
  end

  it "returns an Array containing Symbols that have been created" do
    symbol = "symbol_specs_#{rand(5_000_000)}"
    eval ":#{symbol}"
    Symbol.all_symbols.should include(symbol.to_sym)
  end
end
