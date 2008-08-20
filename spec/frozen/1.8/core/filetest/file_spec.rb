require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/file'

describe "File.file?" do
  it_behaves_like :file_file, :file?, File
end
