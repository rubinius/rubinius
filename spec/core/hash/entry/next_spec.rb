require File.expand_path('../../../../spec_helper', __FILE__)

describe "Hash::Entry#next" do
  it "returns nil if there is no chained entry" do
    Hash::Entry.new(1, 3, 2).next.should be_nil
  end

  it "returns the next entry" do
    a = Hash::Entry.new 1, 2, 3
    b = Hash::Entry.new 4, 5, 6
    a.next = b
    a.next.should == b
  end
end
