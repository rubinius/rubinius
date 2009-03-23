require File.dirname(__FILE__) + '/../spec_helper'

describe "A Cvar node" do
  relates "@@x" do
    parse do
      [:cvar, :@@x]
    end

    compile do |g|
      g.push_scope
      g.push_literal :@@x
      g.send :class_variable_get, 1
    end
  end
end
