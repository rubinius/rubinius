require File.expand_path('../../../../spec_helper', __FILE__)

with_profiler do
  describe "Rubinius::Profiler::Instrumenter#start" do
    before :each do
      @profiler = Rubinius::Profiler::Instrumenter.new
    end

    it "enables the profiler" do
      @profiler.stop.should be_nil
      @profiler.start
      @profiler.stop.should be_kind_of(Rubinius::LookupTable)
    end
  end
end
