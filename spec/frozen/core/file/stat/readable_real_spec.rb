require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/readable_real'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#readable_real?" do
  it_behaves_like :file_readable_real, :readable_real?, FileStat
end
