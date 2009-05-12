describe :hash_equal, :shared => true do
  it "does not compare values when keys don't match" do
    value = mock('x')
    value.should_not_receive(:==)
    value.should_not_receive(:eql?)
    new_hash(1 => value).send(@method, new_hash(2 => value)).should be_false
  end

  it "returns false when the numbers of keys differ without comparing any elements" do
    obj = mock('x')
    h = new_hash(obj => obj)

    obj.should_not_receive(:==)
    obj.should_not_receive(:eql?)

    new_hash.send(@method, h).should be_false
    h.send(@method, new_hash).should be_false
  end

  it "first compares keys via hash" do
    # Can't use should_receive because it uses hash internally
    x = mock('x')
    def x.hash() 0 end
    y = mock('y')
    def y.hash() 0 end

    new_hash(x => 1).send(@method, new_hash(y => 1)).should be_false
  end

  it "does not compare keys with different hash codes via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    x = mock('x')
    y = mock('y')
    x.instance_variable_set(:@other, y)
    y.instance_variable_set(:@other, x)
    def x.eql?(o)
      raise("x Shouldn't receive eql?") if o == @other
      self == o
    end
    def y.eql?(o)
      raise("y Shouldn't receive eql?") if o == @other
      self == o
    end

    def x.hash() 0 end
    def y.hash() 1 end

    new_hash(x => 1).send(@method, new_hash(y => 1)).should be_false
  end

  it "computes equality for recursive hashes" do
    h = new_hash
    h[:a] = h
    h.send(@method, h[:a]).should be_true
    (h == h[:a]).should be_true
  end

  ruby_bug "redmine #2448", "1.9.1" do
    it "computes equality for complex recursive hashes" do
      a, b = {}, {}
      a.merge! :self => a, :other => b
      b.merge! :self => b, :other => a
      a.send(@method, b).should be_true # they both have the same structure!

      c = {}
      c.merge! :other => c, :self => c
      c.send(@method, a).should be_true # subtle, but they both have the same structure!
      a[:delta] = c[:delta] = a
      c.send(@method, a).should be_false # not quite the same structure, as a[:other][:delta] = nil
      c[:delta] = 42
      c.send(@method, a).should be_false
      a[:delta] = 42
      c.send(@method, a).should be_false
      b[:delta] = 42
      c.send(@method, a).should be_true
    end

    it "computes equality for recursive hashes & arrays" do
      x, y, z = [], [], []
      a, b, c = {:foo => x, :bar => 42}, {:foo => y, :bar => 42}, {:foo => z, :bar => 42}
      x << a
      y << c
      z << b
      b.send(@method, c).should be_true # they clearly have the same structure!
      y.send(@method, z).should be_true
      a.send(@method, b).should be_true # subtle, but they both have the same structure!
      x.send(@method, y).should be_true
      y << x
      y.send(@method, z).should be_false
      z << x
      y.send(@method, z).should be_true

      a[:foo], a[:bar] = a[:bar], a[:foo]
      a.send(@method, b).should be_false
      b[:bar] = b[:foo]
      b.send(@method, c).should be_false
    end
  end # ruby_bug
end
