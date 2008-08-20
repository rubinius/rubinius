require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/writable_real'

describe "File.writable_real?" do
  it_behaves_like :file_writable_real, :writable_real?, File
  it_behaves_like :file_writable_real_missing, :writable_real?, File
end
