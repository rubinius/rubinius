require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/zero'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#zero?" do
  it_behaves_like :file_zero, :zero?, FileStat
end
