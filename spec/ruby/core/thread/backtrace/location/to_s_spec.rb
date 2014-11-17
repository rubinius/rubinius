require File.expand_path('../../../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/to_string', __FILE__)

describe 'Thread::Backtrace::Location#to_s' do
  it_behaves_like :thread_backtrace_location_to_string, :to_s
end
