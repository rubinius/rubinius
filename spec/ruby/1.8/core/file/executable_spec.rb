require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/executable'

describe "File.executable?" do
  it_behaves_like :file_executable, :executable?, File
  it_behaves_like :file_executable_missing, :executable?, File
end
