require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/sticky'

describe "FileTest.sticky?" do
  it_behaves_like :file_sticky, :sticky?, FileTest
  it_behaves_like :file_sticky_missing, :sticky?, FileTest
end
