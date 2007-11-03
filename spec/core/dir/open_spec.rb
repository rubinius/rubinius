require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/open'

describe "Dir.open" do
  it_behaves_like :dir_open, :open
end
