require 'erb'
require File.dirname(__FILE__) + '/../../spec_helper'

describe "ERB#filename" do


  it "is reported when error raised" do
    filename = 'foobar.rhtml'
    erb = ERB.new('<% if true %>')   # will raise SyntaxError
    erb.filename = filename
    ex = nil
    proc {
      begin
        erb.result(binding)
      rescue Exception => e
        ex = e
        raise e
      end
    }.should raise_error(SyntaxError)
    expected = filename
    ex.file.should == expected
    ex.line.should == 1
  end


  it "use '(erb)' as filename when filename is not set" do
    erb = ERB.new('<% if true %>')   # will raise SyntaxError
    ex = nil
    proc {
      begin
        erb.result(binding)
      rescue Exception => e
        ex = e
        raise e
      end
    }.should raise_error(SyntaxError)
    expected = '(erb)'
    ex.file.should == expected
    ex.line.should == 1
  end


end
