require File.expand_path('../../../../spec_helper', __FILE__)

describe "Hash::Iterator.new" do
  it "initializes the iterator" do
    h = Hash[:a => 1, :b => 2]
    entries = h.instance_variable_get :@entries
    iter = h.to_iter
    iter.instance_variable_get(:@entries).should == entries
    iter.instance_variable_get(:@capacity).should == entries.size
    iter.instance_variable_get(:@index).should == -1
  end
end
