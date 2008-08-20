require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/chardev'

describe "FileTest.chardev?" do
  it_behaves_like :file_chardev, :chardev?, FileTest
end 
