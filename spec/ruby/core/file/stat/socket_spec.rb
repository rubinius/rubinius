require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/socket'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#socket?" do
  it_behaves_like :file_socket, :socket?, FileStat
end

describe "File::Stat#socket?" do
  it "needs to be reviewed for spec completeness"
end
