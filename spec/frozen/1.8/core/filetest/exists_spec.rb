require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/exist'

describe "FileTest.exists?" do
  it_behaves_like :file_exist, :exists?, FileTest
end
