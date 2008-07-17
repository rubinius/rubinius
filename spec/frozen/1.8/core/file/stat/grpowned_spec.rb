require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/grpowned'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#grpowned?" do
  it_behaves_like :file_grpowned, :grpowned?, FileStat
end
