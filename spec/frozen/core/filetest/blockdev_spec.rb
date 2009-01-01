require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/blockdev'

describe "FileTest.blockdev?" do
  it_behaves_like :file_blockdev, :blockdev?, FileTest
end
