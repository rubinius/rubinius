require File.dirname(__FILE__) + '/../spec_helper'

describe "A False node" do
  relates "false" do
    compile do |g|
      g.push :false
    end
  end
end
