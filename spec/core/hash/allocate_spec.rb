require File.expand_path('../../../spec_helper', __FILE__)

describe "Hash.allocate" do
  # This is very white-box, but we need to be certain
  # it is set up correctly.
  it "initializes the Hash storage" do
    h = Hash.allocate
    h.instance_variable_get(:@size).should == 0
    h.instance_variable_get(:@capacity).should == Hash::MIN_SIZE
    h.instance_variable_get(:@mask).should == Hash::MIN_SIZE - 1
    h.instance_variable_get(:@max_entries).should == Hash::MAX_ENTRIES
    h.instance_variable_get(:@entries).should be_kind_of(Rubinius::Tuple)
  end
end
