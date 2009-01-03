require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/size'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#size?" do
  it_behaves_like :file_size, :size?, FileStat
end
