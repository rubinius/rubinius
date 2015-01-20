require File.expand_path("../spec_helper", __FILE__)

describe "JIT compiling a method call" do
  context "to m()" do
    before :each do
      klass = Class.new do
        def m() :m end
      end

      @o = klass.new

      jit(@o, :m) { @o.m }
    end

    it "compiles" do
      @o.method(:m).executable.jitted?.should be_true
    end

    it "returns the last computed value" do
      @o.m.should equal(:m)
    end

    it "raises an ArgumentError if passed an argument" do
      lambda { @o.m 5 }.should raise_error(ArgumentError)
    end
  end

  context "to m() without calling it" do
    before :each do
      klass = Class.new do
        def m() :m end
      end

      @o = klass.new

      jit(@o, :m)
    end

    it "compiles" do
      @o.method(:m).executable.jitted?.should be_true
    end

    it "returns the last computed value" do
      @o.m.should == :m
    end

    it "raises an ArgumentError if passed an argument" do
      lambda { @o.m 5 }.should raise_error(ArgumentError)
    end
  end

  context "to m(a)" do
    before :each do
      klass = Class.new do
        def m(a) a end
      end

      @o = klass.new

      jit(@o, :m) { @o.m 5 }
    end

    it "compiles" do
      @o.method(:m).executable.jitted?.should be_true
    end

    it "returns the passed argument" do
      @o.m(:a).should == :a
    end

    it "raises an ArgumentError when not passed an argument" do
      lambda { @o.m }.should raise_error(ArgumentError)
    end
  end

  context "to m(*a)" do
    before :each do
      klass = Class.new do
        def m(*a) a end
      end

      @o = klass.new

      jit(@o, :m) { @o.m }
    end

    it "compiles" do
      @o.method(:m).executable.jitted?.should be_true
    end

    it "returns an empty Array when passed no argument" do
      @o.m.should == []
    end

    it "returns a one-element Array when passed one argument" do
      @o.m(1).should == [1]
    end
  end
end
