require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/shared/gmt_offset'

describe "Time#utc_offset" do
  it_behaves_like(:time_gmt_offset, :utc_offset)
end
