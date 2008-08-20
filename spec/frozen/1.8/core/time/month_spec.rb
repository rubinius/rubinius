require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/shared/month'

describe "Time#month" do
  it_behaves_like(:time_month, :month)
end
