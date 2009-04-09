require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Rubinius::Profiler::Instrumenter#show" do
  before :each do
    @stdout, $stdout = $stdout, IOStub.new
    @profiler = Rubinius::Profiler::Instrumenter.new
  end

  after :each do
    $stdout = @stdout
  end

  it "prints out the profile" do
    @profiler.start
    ProfilerSpecs.work 10
    @profiler.stop
    @profiler.show
    $stdout.should =~ %r[ time   seconds   seconds    calls  ms/call  ms/call  name]
    $stdout.should =~ /ProfilerSpecs.work/
  end
end
