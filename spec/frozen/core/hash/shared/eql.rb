describe :hash_eql, :shared => true do

  it "does not compare values when keys don't match" do
    value = mock('x')
    value.should_not_receive(:==)
    value.should_not_receive(:eql?)
    new_hash(1 => value).send(@method, new_hash(2 => value)).should == false
  end

  it "returns false when the numbers of keys differ without comparing any elements" do
    obj = mock('x')
    h = new_hash(obj => obj)

    obj.should_not_receive(:==)
    obj.should_not_receive(:eql?)

    new_hash.send(@method, h).should == false
    h.send(@method, new_hash).should == false
  end

  it "first compares keys via hash" do
    # Can't use should_receive because it uses hash internally
    x = mock('x')
    def x.hash() 0 end
    y = mock('y')
    def y.hash() 0 end

    new_hash(x => 1).send(@method, new_hash(y => 1)).should == false
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

    new_hash(x => 1).send(@method, new_hash(y => 1)).should == false
  end

  it "computes equality for recursive hashes" do
    h = new_hash
    h[:a] = h
    h.send(@method, h[:a]).should == true
    (h == h[:a]).should == true
  end
  
  it "doesn't call to_hash on objects" do
    mock_hash = mock("fake hash")
    def mock_hash.to_hash() new_hash end
    new_hash.send(@method, mock_hash).should be_false
  end
    
  ruby_bug "redmine #2448", "1.9.1" do
    it "computes equality for complex recursive hashes" do
      a, b = {}, {}
      a.merge! :self => a, :other => b
      b.merge! :self => b, :other => a
      a.send(@method, b).should == true # they both have the same structure!

      c = {}
      c.merge! :other => c, :self => c
      c.send(@method, a).should == true # subtle, but they both have the same structure!
      a[:delta] = c[:delta] = a
      c.send(@method, a).should == false # not quite the same structure, as a[:other][:delta] = nil
      c[:delta] = 42
      c.send(@method, a).should == false
      a[:delta] = 42
      c.send(@method, a).should == false
      b[:delta] = 42
      c.send(@method, a).should == true   
    end
  
    it "computes equality for recursive hashes & arrays" do
      x, y, z = [], [], []
      a, b, c = {:foo => x, :bar => 42}, {:foo => y, :bar => 42}, {:foo => z, :bar => 42}
      x << a
      y << c
      z << b
      b.send(@method, c).should == true # they clearly have the same structure!
      y.send(@method, z).should == true
      a.send(@method, b).should == true # subtle, but they both have the same structure!
      x.send(@method, y).should == true
      y << x
      y.send(@method, z).should == false
      z << x
      y.send(@method, z).should == true
    
      a[:foo], a[:bar] = a[:bar], a[:foo]
      a.send(@method, b).should == false
      b[:bar] = b[:foo]
      b.send(@method, c).should == false
    end
  end # ruby_bug
end

# All these tests are true for ==, and for eql? when Ruby >= 1.8.7
describe :hash_eql_additional, :shared => true do
  it "compares values when keys match" do
    x = mock('x')
    y = mock('y')
    def x.==(o) false end
    def y.==(o) false end
    def x.eql?(o) false end
    def y.eql?(o) false end
    new_hash(1 => x).send(@method, new_hash(1 => y)).should == false

    x = mock('x')
    y = mock('y')
    def x.==(o) true end
    def y.==(o) true end
    def x.eql?(o) true end
    def y.eql?(o) true end
    new_hash(1 => x).send(@method, new_hash(1 => y)).should == true
  end
  
  it "compares keys with eql? semantics" do
    new_hash(1.0 => "x").send(@method, new_hash(1.0 => "x")).should == true
    new_hash(1.0 => "x").send(@method, new_hash(1.0 => "x")).should == true
    new_hash(1 => "x").send(@method, new_hash(1.0 => "x")).should == false
    new_hash(1.0 => "x").send(@method, new_hash(1 => "x")).should == false
  end
    
  it "returns true if other Hash has the same number of keys and each key-value pair matches" do
    new_hash(5).send(@method, new_hash(1)).should == true
    new_hash {|h, k| 1}.send(@method, new_hash {}).should == true
    new_hash {|h, k| 1}.send(@method, new_hash(2)).should == true

    a = new_hash(:a => 5)
    b = new_hash
    a.send(@method, b).should == false

    b[:a] = 5
    a.send(@method, b).should == true

    c = new_hash("a" => 5)
    a.send(@method, c).should == false

    d = new_hash {|h, k| 1}
    e = new_hash {}
    d[1] = 2
    e[1] = 2
    d.send(@method, e).should == true
  end
  
  it "does not call to_hash on hash subclasses" do
    new_hash(5 => 6).send(@method, ToHashHash[5 => 6]).should == true
  end

  it "ignores hash class differences" do
    h = new_hash(1 => 2, 3 => 4)
    MyHash[h].send(@method, h).should == true
    MyHash[h].send(@method, MyHash[h]).should == true
    h.send(@method, MyHash[h]).should == true
  end

  # Why isn't this true of eql? too ?
  it "compares keys with matching hash codes via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    a = Array.new(2) do
      obj = mock('0')

      def obj.hash()
        return 0
      end
      # It's undefined whether the impl does a[0].eql?(a[1]) or
      # a[1].eql?(a[0]) so we taint both.
      def obj.eql?(o)
        return true if self == o
        taint
        o.taint
        false
      end

      obj
    end

    new_hash(a[0] => 1).send(@method, new_hash(a[1] => 1)).should == false
    a[0].tainted?.should == true
    a[1].tainted?.should == true

    a = Array.new(2) do
      obj = mock('0')

      def obj.hash()
        # It's undefined whether the impl does a[0].send(@method, a[1]) or
        # a[1].send(@method, a[0]) so we taint both.
        def self.eql?(o) taint; o.taint; true; end
        return 0
      end

      obj
    end

    new_hash(a[0] => 1).send(@method, new_hash(a[1] => 1)).should == true
    a[0].tainted?.should == true
    a[1].tainted?.should == true
  end
end