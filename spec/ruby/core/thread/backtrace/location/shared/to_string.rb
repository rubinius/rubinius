describe :thread_backtrace_location_to_string, :shared => true do
  before :each do
    @frame = ThreadBacktraceLocationSpecs.locations[0]
    @line  = __LINE__ - 1
  end

  it 'converts the call frame to a String' do
    @frame.send(@method).should == "#{__FILE__}:#{@line}:in `__script__'"
  end
end
