require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Hash::Iterator#next" do
  it "returns each non-nil entry from the storage vector" do
    a = [1, 2, 3, nil, 4, nil, 5, nil, nil, nil, 6]
    iter = Hash::Iterator.new a, a.size

    values = []
    while entry = iter.next
      values << entry
    end
    values.should == [1, 2, 3, 4, 5, 6]
  end
end
