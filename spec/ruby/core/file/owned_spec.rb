require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../../../shared/file/owned', __FILE__)

describe "File.owned?" do
  it_behaves_like :file_owned, :owned?, File
end

describe "File.owned?" do
  it "needs to be reviewed for spec completeness"
end
