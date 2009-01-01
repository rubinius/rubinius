require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/conjugate'
require 'complex'

describe "Numeric#conj" do
  it_behaves_like :numeric_conjugate, :conj
end