require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :hash_bucket do
  describe "Hash::Iterator#next" do
    before :each do
      ScratchPad.record []
    end

    ruby_version_is ""..."1.9" do
      it "returns each non-nil entry from the storage vector" do
        h = Hash.new
        a = Hash::Bucket.new 1, 2, 3
        b = Hash::Bucket.new 4, 5, 6
        c = Hash::Bucket.new 7, 8, 9
        e = h.instance_variable_get :@entries
        e[2] = a
        e[4] = b
        e[7] = c

        iter = h.to_iter

        while entry = iter.next(entry)
          ScratchPad << entry
        end
        ScratchPad.recorded.should == [a, b, c]
      end
    end

    ruby_version_is "1.9" do
      it "returns each entry in the Hash" do
        h = Hash[:a, 1, :b, 2, :c, 3]
        iter = h.to_iter

        while entry = iter.next(entry)
          ScratchPad << entry.key
        end
        ScratchPad.recorded.should == [:a, :b, :c]
      end
    end
  end
end

with_feature :hash_hamt do
  describe "Hash::Iterator#next" do
    before :each do
      ScratchPad.record []
    end

    it "returns each entry in the Hash" do
      h = Hash[:a, 1, :b, 2, :c, 3]
      iter = h.to_iter

      while entry = iter.next(entry)
        ScratchPad << entry.key
      end
      ScratchPad.recorded.should == [:a, :b, :c]
    end
  end
end
