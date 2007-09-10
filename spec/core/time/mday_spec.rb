require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/day_spec'

describe "Time#mday" do
  it_behaves_like(@time_day, :mday)
end
