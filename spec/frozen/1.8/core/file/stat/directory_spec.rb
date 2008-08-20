require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/directory'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#directory?" do
  it_behaves_like :file_directory, :directory?, FileStat
end
