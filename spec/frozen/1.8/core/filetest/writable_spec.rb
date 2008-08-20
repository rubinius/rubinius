require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/writable'

describe "FileTest.writable?" do
  it_behaves_like :file_writable, :writable?, FileTest
  it_behaves_like :file_writable_missing, :writable?, FileTest
end
