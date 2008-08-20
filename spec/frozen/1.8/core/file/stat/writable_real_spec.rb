require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/writable_real'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#writable_real?" do
  it_behaves_like :file_writable_real, :writable_real?, FileStat
end
