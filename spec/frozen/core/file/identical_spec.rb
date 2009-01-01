require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/identical'

describe "File.identical?" do
  it_behaves_like :file_identical, :identical?, File
end
