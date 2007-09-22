require File.dirname(__FILE__) + '/../../spec_helper'

@range_begin = shared "Range#begin" do |cmd|
  describe "Range##{cmd}" do
    it "returns the first element of self" do
      (-1..1).send(cmd).should == -1
      (0..1).send(cmd).should == 0
      (0xffff...0xfffff).send(cmd).should == 65535
      ('Q'..'T').send(cmd).should == 'Q'
      ('Q'...'T').send(cmd).should == 'Q'
      (0.5..2.4).send(cmd).should == 0.5
    end
  end
end

describe "Range#begin" do
  it_behaves_like(@range_begin, :begin)
end
