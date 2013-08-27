require File.expand_path('../../../spec_helper', __FILE__)
require 'strscan'

describe "StringScanner#unscan" do
  before :each do
    @s = StringScanner.new("This is a test")
  end

  it "sets the scan pointer to the previous position" do
    @s.scan(/This/)
    @s.unscan
    @s.matched.should == nil
    @s.pos.should == 0
  end

  it "remember only one previous position" do
    pos1 = @s.pos
    @s.scan(/This/)
    pos2 = @s.pos
    @s.scan(/ is/)
    @s.unscan
    @s.pos.should == pos2
  end

  it "raises a ScanError when the previous match had failed" do
    lambda { @s.unscan }.should raise_error(ScanError)
    lambda { @s.scan(/\d/); @s.unscan }.should raise_error(ScanError)
  end
end
