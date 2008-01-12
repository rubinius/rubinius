require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/directory'

describe "File.directory?" do
  it_behaves_like :file_directory, :directory?, File
end
