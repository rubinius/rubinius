require File.dirname(__FILE__) + '/../../spec_helper'

@range_end = shared "Range#end" do |cmd|
  describe "Range##{cmd}" do
    it "end returns the last element of self" do
      (-1..1).send(cmd).should == 1
      (0..1).send(cmd).should == 1
      ("A".."Q").send(cmd).should == "Q"
      ("A"..."Q").send(cmd).should == "Q"
      (0xffff...0xfffff).send(cmd).should == 1048575
      (0.5..2.4).send(cmd).should == 2.4
    end
  end
end

describe "Range#end" do
  it_behaves_like(@range_end, :end)
end
