require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/month_spec'

describe "Time#mon" do
  it_behaves_like(time_month, :mon)
end
