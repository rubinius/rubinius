require File.expand_path('../../../spec_helper', __FILE__)

describe 'Process::Status#initialize' do
  it 'initializes a new Process::Status instance' do
    Process::Status.new.should be_an_instance_of(Process::Status)
  end
end
