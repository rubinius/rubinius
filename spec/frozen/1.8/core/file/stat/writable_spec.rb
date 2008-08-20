require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/writable'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#writable?" do
  it_behaves_like :file_writable, :writable?, FileStat
end
