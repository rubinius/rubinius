require File.expand_path('../../../../spec_helper', __FILE__)

describe "Hash::Entry.new" do
  it "creates a new Entry" do
    b = Hash::Entry.new(1, 2, 3)
    b.key.should == 1
    b.key_hash.should == 2
    b.value.should == 3
    b.next.should be_nil
  end
end
