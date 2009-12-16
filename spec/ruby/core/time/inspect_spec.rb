require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/shared/inspect'

describe "Time.inspect" do
  it_behaves_like :inspect, :inspect
end
