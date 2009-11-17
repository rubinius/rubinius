require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/sticky'

describe "File.sticky?" do
  it_behaves_like :file_sticky, :sticky?, File
  it_behaves_like :file_sticky_missing, :sticky?, File
end

describe "File.sticky?" do
  it "needs to be reviewed for spec completeness"
end
