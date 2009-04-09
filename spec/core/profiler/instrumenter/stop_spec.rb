require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Rubinius::Profiler::Instrumenter#stop" do
  before :each do
    @profiler = Rubinius::Profiler::Instrumenter.new
  end

  it "disables the profiler" do
    @profiler.start
    @profiler.stop.should be_kind_of(LookupTable)
    @profiler.stop.should be_nil
  end
end
