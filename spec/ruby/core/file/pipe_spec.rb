require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../../../shared/file/pipe', __FILE__)

describe "File.pipe?" do
  it_behaves_like :file_pipe, :pipe?, File
end

describe "File.pipe?" do
  it "needs to be reviewed for spec completeness"
end
