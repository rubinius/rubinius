require File.dirname(__FILE__) + '/../spec_helper'

describe "A False node" do
  relates "false" do
    parse do
      [:false]
    end

    compile do |g|
      g.push :false
    end
  end
end
