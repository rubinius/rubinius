require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/shared/gmtime'

describe "Time#gmtime" do
  it_behaves_like(:time_gmtime, :gmtime)
end
