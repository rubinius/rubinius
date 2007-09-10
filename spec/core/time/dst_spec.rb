require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/isdst_spec'

describe "Time#dst?" do
  it_behaves_like(@time_isdst, :dst?)
end
