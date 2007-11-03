require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/exist'

describe "FileTest.exists?" do
  it_behaves_like(:filetest_exist, :exists?)
end
