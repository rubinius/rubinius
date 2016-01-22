describe :hash_comparison, shared: true do
  it "raises a TypeError if the right operand is not a hash" do
    lambda { new_hash(a: 1).send(@method, 1) }.should raise_error(TypeError)
    lambda { new_hash(a: 1).send(@method, nil) }.should raise_error(TypeError)
    lambda { new_hash(a: 1).send(@method, []) }.should raise_error(TypeError)
  end

  it "returns false if both hashes have the same keys but different values" do
    h1 = new_hash(a: 1)
    h2 = new_hash(a: 2)

    h1.send(@method, h2).should be_false
    h2.send(@method, h1).should be_false
  end
end
