require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/process/times'

describe "Process.times" do
  it_behaves_like :process_times, :times, Process
end
