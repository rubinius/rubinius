require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Rubinius::Profiler::Instrumenter#start" do
  before :each do
    @profiler = Rubinius::Profiler::Instrumenter.new
  end

  it "enables the profiler" do
    @profiler.stop.should be_nil
    @profiler.start
    @profiler.stop.should be_kind_of(LookupTable)
  end
end
