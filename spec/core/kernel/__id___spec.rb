require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/object_id_spec'

describe "Kernel#__id__" do
  it_behaves_like(@kernel_object_id, :__id__) 
end
