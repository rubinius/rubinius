require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/executable'

describe "FileTest.executable?" do
  it_behaves_like :file_executable, :executable?, FileTest
  it_behaves_like :file_executable_missing, :executable?, FileTest
end
