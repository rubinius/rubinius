require File.expand_path('../../../../spec_helper', __FILE__)

with_profiler do
  describe "Rubinius::Profiler::Instrumenter#stop" do
    before :each do
      @profiler = Rubinius::Profiler::Instrumenter.new
    end

    it "disables the profiler" do
      @profiler.start
      @profiler.stop.should be_kind_of(Rubinius::LookupTable)
      @profiler.stop.should be_nil
    end
  end
end
