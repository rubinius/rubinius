require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/executable'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#executable?" do
  it_behaves_like :file_executable, :executable?, FileStat
end
