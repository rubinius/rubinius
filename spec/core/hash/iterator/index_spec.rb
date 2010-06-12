require File.expand_path('../../../../spec_helper', __FILE__)

describe "Hash::Iterator#index" do
  it "returns the index of the last object returned by #next" do
    h = Hash.new
    a = Hash::Entry.new 1, 2, 3
    b = Hash::Entry.new 4, 5, 6
    e = h.instance_variable_get :@entries
    e[2] = a
    e[4] = b

    iter = h.to_iter

    (entry = iter.next(entry)).should == a
    (entry = iter.next(entry)).should == b

    iter.index.should == 4
  end
end
