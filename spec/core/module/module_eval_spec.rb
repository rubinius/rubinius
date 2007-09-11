require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/class_eval_spec'

describe "Module#module_eval" do
  it_behaves_like @module_class_eval, :module_eval
end
