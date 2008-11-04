require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash.allocate" do
  # This is very white-box, but we need to be certain
  # it is set up correctly.
  it "initializes the Hash storage" do
    h = Hash.allocate
    h.instance_variable_get(:@count).should == 0
    h.instance_variable_get(:@records).should == Hash::MIN_SIZE
    h.instance_variable_get(:@bins).should be_kind_of(Tuple)
  end
end
