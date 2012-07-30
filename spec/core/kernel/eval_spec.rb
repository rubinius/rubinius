require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#eval" do
  before :each do
    @cache = tmp("eval_cache.rbc")
    name = fixture(__FILE__, "eval_cache.rb")
    KernelSpecs.cache_file name, @cache
  end

  after :each do
    rm_r @cache
  end

  it "creates a CompiledCode that can be cached and re-run" do
    KernelSpecs.run_cache(@cache).should == "Object:Object"
  end

  describe "when line-number differs" do
    it "does not use the cache" do
      eval("__LINE__", binding, "(file)", 1).should == 1
      eval("__LINE__", binding, "(file)", 2).should == 2
    end
  end
end
