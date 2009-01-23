require File.dirname(__FILE__) + '/../spec_helper'

describe "A True node" do
  relates "true" do
    parse do
      [:true]
    end

    compile do |g|
      g.push :true
    end
  end
end
