require File.dirname(__FILE__) + '/../spec_helper'

describe "A True node" do
  relates "true" do
    compile do |g|
      g.push :true
    end
  end
end
