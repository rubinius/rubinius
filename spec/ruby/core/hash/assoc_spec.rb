require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Hash#assoc" do
    before(:each) do
      @h = new_hash(:a => 1, :b => 2, :c => 1, :d => 3)
    end

    it "returns a 2-element Array if the argument is == to a key of the Hash" do
      @h.assoc(:b).should == [:b, 2]
    end

    it "only returns the first matching key-value pair for identity hashes" do
      h = new_hash
      h.compare_by_identity

      h["a"] = 1
      h["a"] = 2
      h.size.should == 2

      h.assoc("a").should == ["a", 1]
    end

    it "uses #== to compare the argument to the keys" do
      key = mock("Hash#assoc key")
      key.should_receive(:==).with(:a).and_return(true)

      @h.assoc(key).should == [:a, 1]
    end

    it "returns nil if the argument is not a key of the Hash" do
      @h.assoc(:e).should be_nil
    end
  end
end
