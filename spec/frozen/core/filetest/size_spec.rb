require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/size'

describe "FileTest.size?" do
  it_behaves_like :file_size,                     :size?, FileTest
  it_behaves_like :file_size_nil_when_missing,    :size?, FileTest
  it_behaves_like :file_size_nil_when_empty,      :size?, FileTest
  it_behaves_like :file_size_with_file_argument,  :size?, FileTest
end

describe "FileTest.size" do
  it_behaves_like :file_size,                     :size,  FileTest
  it_behaves_like :file_size_raise_when_missing,  :size,  FileTest
  it_behaves_like :file_size_0_when_empty,        :size,  FileTest
  it_behaves_like :file_size_with_file_argument,  :size,  FileTest
end
