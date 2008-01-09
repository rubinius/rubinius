require File.dirname(__FILE__) + '/../../spec_helper'

describe Object, '=~' do
  it 'returns false matching any object' do
    o = Object.new

    (o =~ /Object/).should == false
    (o =~ 'Object').should == false
    (o =~ Object).should == false
    (o =~ Object.new).should == false
    (o =~ nil).should == false
    (o =~ true).should == false
  end
end
