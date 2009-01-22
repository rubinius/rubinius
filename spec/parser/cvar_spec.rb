require File.dirname(__FILE__) + '/../spec_helper'

describe "A Cvar node" do
  relates "@@x" do
    parse do
      [:cvar, :@@x]
    end

    # cvar
  end
end
