require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/size'

describe "FileTest.size?" do
  it_behaves_like :file_size, :size?, FileTest
  it_behaves_like :file_size_missing, :size?, FileTest
end
