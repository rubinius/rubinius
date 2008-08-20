require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/blockdev'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#blockdev?" do
  it_behaves_like :file_blockdev, :blockdev?, FileStat
end
