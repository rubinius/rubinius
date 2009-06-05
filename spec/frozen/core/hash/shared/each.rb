describe :hash_each, :shared => true do
    it "yields a [[key, value]] Array for each pair to a block expecting |*args|" do
      all_args = []
      new_hash(1 => 2, 3 => 4).send(@method) { |*args| all_args << args }
      all_args.sort.should == [[[1, 2]], [[3, 4]]]
    end

  it "yields the key and value of each pair to a block expecting |key, value|" do
    r = new_hash
    h = new_hash(:a => 1, :b => 2, :c => 3, :d => 5)
    h.send(@method) { |k,v| r[k.to_s] = v.to_s }.should equal(h)
    r.should == new_hash("a" => "1", "b" => "2", "c" => "3", "d" => "5")
  end

  it "uses the same order as keys() and values()" do
    h = new_hash(:a => 1, :b => 2, :c => 3, :d => 5)
    keys = []
    values = []

    h.send(@method) do |k, v|
      keys << k
      values << v
    end

    keys.should == h.keys
    values.should == h.values
  end
end  
