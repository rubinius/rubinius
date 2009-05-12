describe :array_eql, :shared => true do
  it "returns true if other is the same array" do
    a = [1]
    a.send(@method, a).should be_true
  end

  it "returns true if corresponding elements are #eql?" do
    [].send(@method, []).should be_true
    [1, 2, 3, 4].send(@method, [1, 2, 3, 4]).should be_true
  end

  it "returns false if other is shorter than self" do
    [1, 2, 3, 4].send(@method, [1, 2, 3]).should be_false
  end

  it "returns false if other is longer than self" do
    [1, 2, 3, 4].send(@method, [1, 2, 3, 4, 5]).should be_false
  end

  it "returns false immediately when sizes of the arrays differ" do
    obj = mock('1')
    obj.should_not_receive(@method)

    []        .send(@method,    [obj]  ).should be_false
    [obj]     .send(@method,    []     ).should be_false
  end

  ruby_bug "ruby-core #1448", "1.9.1" do
    it "handles well recursive arrays" do
      a = ArraySpecs.empty_recursive_array
      a       .send(@method,    [a]    ).should be_true
      a       .send(@method,    [[a]]  ).should be_true
      [a]     .send(@method,    a      ).should be_true
      [[a]]   .send(@method,    a      ).should be_true

      a2 = ArraySpecs.empty_recursive_array
      a       .send(@method,    a2     ).should be_true
      a       .send(@method,    [a2]   ).should be_true
      a       .send(@method,    [[a2]] ).should be_true
      [a]     .send(@method,    a2     ).should be_true
      [[a]]   .send(@method,    a2     ).should be_true

      back = []
      forth = [back]; back << forth;
      back   .send(@method,  a  ).should be_true

      x = []; x << x << x
      x       .send(@method,    a                ).should be_false
      x       .send(@method,    [a, a]           ).should be_false
      x       .send(@method,    [x, a]           ).should be_false
      [x, a]  .send(@method,    [a, x]           ).should be_false
      x       .send(@method,    [x, x]           ).should be_true
      x       .send(@method,    [[x, x], [x, x]] ).should be_true

      tree = [];
      branch = []; branch << tree << tree; tree << branch
      tree2 = [];
      branch2 = []; branch2 << tree2 << tree2; tree2 << branch2
      forest = [tree, branch, :bird, a]; forest << forest
      forest2 = [tree2, branch2, :bird, a2]; forest2 << forest2

      forest .send(@method,     forest2         ).should be_true
      forest .send(@method,     [tree2, branch, :bird, a, forest2]).should be_true

      diffforest = [branch2, tree2, :bird, a2]; diffforest << forest2
      forest .send(@method,     diffforest      ).should be_false
      forest .send(@method,     [nil]           ).should be_false
      forest .send(@method,     [forest]        ).should be_false
    end
  end

  it "does not call #to_ary on its argument" do
    obj = mock('to_ary')
    obj.should_not_receive(:to_ary)

    [1, 2, 3].send(@method, obj).should be_false
  end

  it "does not call #to_ary on Array subclasses" do
    ary = ArraySpecs::ToAryArray[5, 6, 7]
    ary.should_not_receive(:to_ary)
    [5, 6, 7].send(@method, ary).should be_true
  end

  it "ignores array class differences" do
    ArraySpecs::MyArray[1, 2, 3].send(@method, [1, 2, 3]).should be_true
    ArraySpecs::MyArray[1, 2, 3].send(@method, ArraySpecs::MyArray[1, 2, 3]).should be_true
    [1, 2, 3].send(@method, ArraySpecs::MyArray[1, 2, 3]).should be_true
  end
end
