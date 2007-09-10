require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/gmt_offset_spec'

describe "Time#gmtoff" do
  it_behaves_like(@time_gmt_offset, :gmtoff)
end
