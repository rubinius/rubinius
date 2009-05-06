describe :hash_store, :shared => true do
  it "associates the key with the value and return the value" do
    h = new_hash(:a => 1)
    h.send(@method, :b, 2).should == 2
    h.should == new_hash(:b=>2, :a=>1)
  end

  it "duplicates string keys using dup semantics" do
    # dup doesn't copy singleton methods
    key = "foo"
    def key.reverse() "bar" end
    h = new_hash
    h.send(@method, key, 0)
    h.keys[0].reverse.should == "oof"
  end

  it "stores unequal keys that hash to the same value" do
    h = new_hash
    k1 = ["x"]
    k2 = ["y"]
    # So they end up in the same bucket
    def k1.hash() 0 end
    def k2.hash() 0 end

    h[k1] = 1
    h[k2] = 2
    h.size.should == 2
  end

  it "duplicates and freezes string keys" do
    key = "foo"
    h = new_hash
    h.send(@method, key, 0)
    key << "bar"

    h.should == new_hash("foo" => 0)
    h.keys[0].frozen?.should == true
  end

  it "raises a TypeError if called on a frozen instance" do
    lambda { HashSpecs.frozen_hash.send(@method, 1, 2) }.should raise_error(TypeError)
  end
end
