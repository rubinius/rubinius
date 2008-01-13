require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/writable'

describe "File.writable?" do
  it_behaves_like :file_writable, :writable?, File
  it_behaves_like :file_writable_missing, :writable?, File
end
