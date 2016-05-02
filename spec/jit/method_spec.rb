require File.expand_path("../spec_helper", __FILE__)

with_feature :jit do
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

    context "to m(a: 0)" do
      before :each do
        klass = Class.new do
          def m(a: 0) a end
        end

        @o = klass.new

        jit(@o, :m) { @o.m }
      end

      it "compiles" do
        @o.method(:m).executable.jitted?.should be_true
      end

      it "returns the default keyword value when passed no arguments" do
        @o.m.should == 0
      end

      it "returns the passed value of the keyword" do
        @o.m(a: 2).should == 2
      end

      it "raises an ArgumentError when passed a non-matching keyword argument" do
        lambda { @o.m(b: 2) }.should raise_error(ArgumentError)
      end

      it "raises an ArgumentError when passed extra keyword arguments" do
        lambda { @o.m(a: 2, b: 3) }.should raise_error(ArgumentError)
      end
    end

    context "to m(a=1, **kw)" do
      before :each do
        klass = Class.new do
          def m(a=1, **kw) [a, kw] end
        end

        @o = klass.new

        jit(@o, :m) { @o.m }
      end

      it "compiles" do
        @o.method(:m).executable.jitted?.should be_true
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

    context "to m(a:, b: 'b', **) super end" do
      before :each do
        parent = Class.new do
          def m(**args) args end
        end

        child = Class.new(parent) do
          def m(a:, b: 'b', **) super end
        end

        @o = child.new

        jit(@o, :m) { @o.m(a: "a", c: "c") }
      end

      it "compiles" do
        @o.method(:m).executable.jitted?.should be_true
      end

      it "returns all keyword arguments passed including default ones" do
        @o.m(a: "a", c: "c").should == {a: "a", b: "b", c: "c"}
      end
    end
  end
end
