require File.expand_path("../spec_helper", __FILE__)

with_feature :jit do
  describe "JIT compiling a string_dup instruction" do
    before :each do
      klass = Class.new do
        def m() "abc" end
      end

      @o = klass.new

      jit(@o, :m) { @o.m }
    end

    it "compiles" do
      @o.method(:m).executable.jitted?.should be_true
    end

    it "dups the literal String" do
      @o.method(:m).executable.literals[0].object_id.should_not equal(@o.m)
    end
  end
end
