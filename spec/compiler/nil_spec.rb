require File.dirname(__FILE__) + '/../spec_helper'

describe "A Nil node" do
  relates "nil" do
    compile do |g|
      g.push :nil
    end
  end
end
