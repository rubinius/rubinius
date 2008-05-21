require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/directory'

describe "FileTest.directory?" do
  it_behaves_like :file_directory, :directory?, FileTest
end
