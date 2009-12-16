require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/shared/inspect'

describe "Time.to_s" do
  it_behaves_like :inspect, :to_s
end
