require File.dirname(__FILE__) + '/../spec_helper'

describe "A Postexe node" do
  relates "END { 1 }" do
    parse do
      [:iter, [:postexe], nil, [:lit, 1]]
    end

    # postexe
  end
end
