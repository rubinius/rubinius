require File.dirname(__FILE__) + '/../../spec_helper'

@range_eql = shared "Range#eql?" do |cmd|
  describe "Range##{cmd}" do
    it "returns true if other has same begin, end, and exclude_end?" do
      (0..2).send(cmd, 0..2).should == true
      (5..10).send(cmd, Range.new(5,10)).should == true
      (1482..1911).send(cmd, 1482...1911).should == false
      ('G'..'M').send(cmd,'G'..'M').should == true
      ('D'..'V').send(cmd, Range.new('D','V')).should == true
      ('Q'..'X').send(cmd, 'Q'...'X').should == false
      (0xffff..0xfffff).send(cmd, 0xffff..0xfffff).should == true
      (0xffff..0xfffff).send(cmd, Range.new(0xffff,0xfffff)).should == true
      (0xffff..0xfffff).send(cmd, 0xffff...0xfffff).should == false
      (0.5..2.4).send(cmd, 0.5..2.4).should == true
      (0.5..2.4).send(cmd, Range.new(0.5, 2.4)).should == true
      (0.5..2.4).send(cmd, 0.5...2.4).should == false
    end
  end
end

describe "Range#eql?" do
  it_behaves_like(@range_eql, :eql?)
end
