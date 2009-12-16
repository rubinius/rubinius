require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/executable_real'

describe "FileTest.executable_real?" do
  it_behaves_like :file_executable_real, :executable_real?, FileTest
  it_behaves_like :file_executable_real_missing, :executable_real?, FileTest
end
