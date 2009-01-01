require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/socket'

describe "FileTest.socket?" do
  it_behaves_like :file_socket, :socket?, FileTest
end
