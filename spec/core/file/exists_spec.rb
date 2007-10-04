require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/exist_spec.rb'

describe "File.exists?" do
  it_behaves_like(@file_exist, :exists?)
end
