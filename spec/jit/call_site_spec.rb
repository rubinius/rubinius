require File.expand_path("../spec_helper", __FILE__)

with_feature :jit do
  describe "JIT compiling a call site" do
    context "to m()" do
      before :each do
        klass = Class.new do
          def m() :m end
          def call() m() end
        end

        @o = klass.new

        jit(@o, :call) { @o.call }
      end

      it "compiles" do
        @o.method(:call).executable.jitted?.should be_true
      end

      it "returns the result of calling the method" do
        @o.call.should equal(:m)
      end
    end

    context "to m() defined in an included module" do
      before :each do
        mod = Module.new do
          def m() :m end
        end
        klass = Class.new do
          def call() m() end
          include mod
        end

        @o = klass.new

        jit(@o, :call) { @o.call }
      end

      it "compiles" do
        @o.method(:call).executable.jitted?.should be_true
      end

      it "returns the result of calling the included method" do
        @o.call.should equal(:m)
      end
    end

    context "to m() defined in a prepended module" do
      before :each do

        mod = Module.new do
          def m() :m end
        end
        klass = Class.new do
          def m() :shadowed end
          def call() m() end
          prepend mod
        end

        o = @o = klass.new

        jit(o, :call) { o.call }
      end

      it "compiles" do
        @o.method(:call).executable.jitted?.should be_true
      end

      it "returns the result of calling the prepended method" do
        @o.call.should equal(:m)
      end
    end

    context "to m() that calls super defined in a prepended module" do
      before :each do

        mod = Module.new do
          def m() super; :m end
        end
        klass = Class.new do
          def m() :shadowed end
          def call() m() end
          prepend mod
        end

        o = @o = klass.new

        jit(o, :call) { o.call }
      end

      it "compiles" do
        @o.method(:call).executable.jitted?.should be_true
      end

      it "returns the result of calling the prepended method" do
        @o.call.should equal(:m)
      end
    end
  end
end
