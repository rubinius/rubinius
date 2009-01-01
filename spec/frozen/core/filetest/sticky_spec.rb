require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/sticky'

describe "FileTest.sticky?" do
  it_behaves_like :file_sticky, :sticky?, FileTest
end
