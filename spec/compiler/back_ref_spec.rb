require File.dirname(__FILE__) + '/../spec_helper'

describe "An Back_ref node" do
  relates "[$&, $`, $', $+]" do
    compile do |g|
      g.push_variables
      g.push_literal :"&"
      g.send :back_ref, 1

      g.push_variables
      g.push_literal :"`"
      g.send :back_ref, 1

      g.push_variables
      g.push_literal :"'"
      g.send :back_ref, 1

      g.push_variables
      g.push_literal :"+"
      g.send :back_ref, 1

      g.make_array 4
    end
  end
end
