require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../../../shared/file/sticky', __FILE__)

describe "File.sticky?" do
  it_behaves_like :file_sticky, :sticky?, File
  it_behaves_like :file_sticky_missing, :sticky?, File
end

describe "File.sticky?" do
  it "needs to be reviewed for spec completeness"
end
