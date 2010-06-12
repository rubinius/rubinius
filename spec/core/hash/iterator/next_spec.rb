require File.expand_path('../../../../spec_helper', __FILE__)

describe "Hash::Iterator#next" do
  it "returns each non-nil entry from the storage vector" do
    h = Hash.new
    a = Hash::Entry.new 1, 2, 3
    b = Hash::Entry.new 4, 5, 6
    c = Hash::Entry.new 7, 8, 9
    e = h.instance_variable_get :@entries
    e[2] = a
    e[4] = b
    e[7] = c

    iter = h.to_iter

    values = []
    while entry = iter.next(entry)
      values << entry
    end
    values.should == [a, b, c]
  end
end
