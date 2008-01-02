require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/delete'

describe "Dir.rmdir" do
  it_behaves_like :dir_delete, :rmdir
end
