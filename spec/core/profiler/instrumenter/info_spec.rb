require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

with_profiler do
  describe "Rubinius::Profiler::Instrumenter#info" do
    before :each do
      @profiler = Rubinius::Profiler::Instrumenter.new
    end

    it "returns the profile data" do
      @profiler.start
      ProfilerSpecs.work 10
      @profiler.stop

      data = @profiler.info
      data.should be_kind_of(Rubinius::LookupTable)
      data.keys.should include(:method, :methods)
    end
  end
end
