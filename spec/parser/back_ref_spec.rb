require File.dirname(__FILE__) + '/../spec_helper'

describe "An Back_ref node" do
  relates "[$&, $`, $', $+]" do
    parse do
      [:array,
       [:back_ref, :&],
       [:back_ref, :`],
       [:back_ref, :"'"],
       [:back_ref, :+]]
    end

    # back_ref
  end
end
