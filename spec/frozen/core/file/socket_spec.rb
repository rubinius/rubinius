require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/socket'

describe "File.socket?" do
  it_behaves_like :file_socket, :socket?, File
end

describe "File.socket?" do
  it "needs to be reviewed for spec completeness"
end
