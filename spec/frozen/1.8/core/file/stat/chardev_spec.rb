require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/chardev'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#chardev?" do
  it_behaves_like :file_chardev, :chardev?, FileStat
end 
