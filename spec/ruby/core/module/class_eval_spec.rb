require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/class_eval'

describe "Module#class_eval" do
  it_behaves_like :module_class_eval, :class_eval
end
