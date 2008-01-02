require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/shared/day'

describe "Time#mday" do
  it_behaves_like(:time_day, :mday)
end
