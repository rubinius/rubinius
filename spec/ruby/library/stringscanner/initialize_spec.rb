require File.expand_path('../../../spec_helper', __FILE__)
require 'strscan'

describe "StringScanner#initialize" do
  before :each do
    @s = StringScanner.new("This is a test")
  end

  it "is a private method" do
    StringScanner.should have_private_instance_method(:initialize)
  end

  it "returns an instance of StringScanner" do
    @s.should be_kind_of(StringScanner)
    @s.tainted?.should be_false
    @s.eos?.should be_false
  end
end
