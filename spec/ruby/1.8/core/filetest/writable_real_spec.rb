require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/writable_real'

describe "FileTest.writable_real?" do
  it_behaves_like :file_writable_real, :writable_real?, FileTest
  it_behaves_like :file_writable_real_missing, :writable_real?, FileTest
end
