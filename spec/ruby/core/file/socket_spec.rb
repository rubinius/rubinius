require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../../../shared/file/socket', __FILE__)

describe "File.socket?" do
  it_behaves_like :file_socket, :socket?, File
end

describe "File.socket?" do
  it "needs to be reviewed for spec completeness"
end
