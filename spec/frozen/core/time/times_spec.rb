require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/process/times'

describe 'Time.times' do
  ruby_version_is "" ... "1.9" do
    it_behaves_like :process_times, :times, Time
  end
end
