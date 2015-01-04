require File.expand_path('../../../spec_helper', __FILE__)

describe 'Process::Status#exitstatus' do
  it 'returns the exit status as a Fixnum' do
    Process::Status.new(42, 1).exitstatus.should == 1
  end
end
