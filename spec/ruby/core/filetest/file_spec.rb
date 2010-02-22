require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../../../shared/file/file', __FILE__)

describe "File.file?" do
  it_behaves_like :file_file, :file?, File
end
