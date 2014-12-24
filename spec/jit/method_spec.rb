require File.expand_path("../spec_helper", __FILE__)

describe "JIT compiling a method call" do
  context "to m(a)" do
    before :each do
      klass = Class.new do
        def m(a) a end
      end

      @o = klass.new

      warmup { @o.m 5 }
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

      warmup { @o.m }
    end

    it "returns an empty Array when passed no argument" do
      @o.m.should == []
    end

    it "returns a one-element Array when passed one argument" do
      @o.m(1).should == [1]
    end
  end

  context "to m(a=1, **kw)" do
    before :each do
      klass = Class.new do
        def m(a=1 **kw) [a, kw] end
      end

      @o = klass.new

      warmup { @o.m }
    end

    context "when passed one argument" do
      it "assigns Symbol keys to the keyword rest argument" do
        @o.m(a: 1, b: 2).should == [1, {a: 1, b: 2}]
      end

      it "assigns non-Symbol keys to the default argument" do
        @o.m("a" => 1, b: 2).should == [{"a" => 1}, {b: 2}]
      end
    end
  end
end
