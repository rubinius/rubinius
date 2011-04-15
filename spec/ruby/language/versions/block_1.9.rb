describe "Block-local variables" do
  # Examples phrased so the concatenation of the describe and it blocks make
  # grammatical sense.
  it "are introduced with a semi-colon in the parameter list" do
    lambda { [1].each {|one; bl| } }.should_not raise_error(SyntaxError)
  end

  it "can be specified in a comma-separated list after the semi-colon" do
    lambda { [1].each {|one; bl, bl2| } }.should_not raise_error(SyntaxError)
  end

  it "can not have the same name as one of the standard parameters" do
    lambda { eval "[1].each {|foo; foo| }" }.should raise_error(SyntaxError)
    lambda { eval "[1].each {|foo, bar; glark, bar| }" }.should raise_error(SyntaxError)
  end

  it "can not be prefixed with an asterisk" do
    lambda { eval "[1].each {|foo; *bar| }" }.should raise_error(SyntaxError)
    lambda do
      eval "[1].each {|foo, bar; glark, *fnord| }"
    end.should raise_error(SyntaxError)
  end

  it "can not be prefixed with an ampersand" do
    lambda { eval "[1].each {|foo; &bar| }" }.should raise_error(SyntaxError)
    lambda do
      eval "[1].each {|foo, bar; glark, &fnord| }"
    end.should raise_error(SyntaxError)
  end

  it "can not be assigned default values" do
    lambda { eval "[1].each {|foo; bar=1| }" }.should raise_error(SyntaxError)
    lambda do
      eval "[1].each {|foo, bar; glark, fnord=:fnord| }"
    end.should raise_error(SyntaxError)
  end

  it "need not be preceeded by standard parameters" do
    lambda { [1].each {|; foo| } }.should_not raise_error(SyntaxError)
    lambda { [1].each {|; glark, bar| } }.should_not raise_error(SyntaxError)
  end

  it "only allow a single semi-colon in the parameter list" do
    lambda { eval "[1].each {|foo; bar; glark| }" }.should raise_error(SyntaxError)
    lambda { eval "[1].each {|; bar; glark| }" }.should raise_error(SyntaxError)
  end

  it "override shadowed variables from the outer scope" do
    out = :out
    [1].each {|; out| out = :in }
    out.should == :out

    a = :a
    b = :b
    c = :c
    d = :d
    {:ant => :bee}.each_pair do |a, b; c, d|
      a = :A
      b = :B
      c = :C
      d = :D
    end
    a.should == :a
    b.should == :b
    c.should == :c
    d.should == :d
  end

  it "are not automatically instantiated in the outer scope" do
    defined?(glark).should be_nil
    [1].each {|;glark| 1}
    defined?(glark).should be_nil
  end

  it "are automatically instantiated in the block" do
    [1].each do |;glark|
      glark.should be_nil
    end
  end
end
