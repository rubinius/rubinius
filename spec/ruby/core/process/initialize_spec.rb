require File.expand_path('../../../spec_helper', __FILE__)

describe 'Process::Status#initialize' do
  it 'initializes a new Process::Status instance' do
    Process::Status.new.should be_an_instance_of(Process::Status)
  end

  it 'initializes Process::Status with a PID' do
    status = Process::Status.new(42)

    status.pid.should == 42
  end

  it 'initializes Process::Status with a PID and status' do
    status = Process::Status.new(42, 1)

    status.exitstatus.should == 1
  end

  it 'initializes Process::Status with a PID, status and term signal' do
    status = Process::Status.new(42, 1, 2)

    status.termsig.should == 2
  end

  it 'initializes Process::Status with a PID, status, term signal and stop signal' do
    status = Process::Status.new(42, 1, 2, 3)

    status.stopsig.should == 3
  end
end
