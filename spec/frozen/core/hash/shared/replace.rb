describe :hash_replace, :shared => true do
  it "replaces the contents of self with other" do
    h = { :a => 1, :b => 2 }
    h.send(@method, :c => -1, :d => -2).should equal(h)
    h.should == { :c => -1, :d => -2 }
  end

  it "tries to convert the passed argument to a hash using #to_hash" do
    obj = mock('{1=>2,3=>4}')
    obj.should_receive(:to_hash).and_return({1 => 2, 3 => 4})

    h = {}
    h.send(@method, obj)
    h.should == {1 => 2, 3 => 4}
  end

  it "calls to_hash on hash subclasses" do
    h = {}
    h.send(@method, ToHashHash[1 => 2])
    h.should == {1 => 2}
  end

  it "does not transfer default values" do
    hash_a = {}
    hash_b = Hash.new(5)
    hash_a.send(@method, hash_b)
    hash_a.default.should == 5

    hash_a = {}
    hash_b = Hash.new { |h, k| k * 2 }
    hash_a.send(@method, hash_b)
    hash_a.default(5).should == 10

    hash_a = Hash.new { |h, k| k * 5 }
    hash_b = Hash.new(lambda { raise "Should not invoke lambda" })
    hash_a.send(@method, hash_b)
    hash_a.default.should == hash_b.default
  end

  it "raises a TypeError if called on a frozen instance" do
    HashSpecs.frozen_hash.send(@method, HashSpecs.frozen_hash) # ok, nothing changed
    block = lambda { HashSpecs.frozen_hash.send(@method, HashSpecs.empty_frozen_hash) }
    block.should raise_error(TypeError)
  end
end
