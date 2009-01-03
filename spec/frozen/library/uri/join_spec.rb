require File.dirname(__FILE__) + '/../../spec_helper'
require 'uri'

describe "URI.join" do
  it "returns a URI object of the concatonation of a protocol and domain, and a path" do
    URI.join("http://localhost/","main.rbx").should == URI.parse("http://localhost/main.rbx")
  end
  
  it "doesn't create redundent '/'s" do
    URI.join("http://localhost/", "/main.rbx").should == URI.parse("http://localhost/main.rbx")
  end
end


# assert_equal(URI.parse('http://foo/bar'), URI.join('http://foo/bar'))
# assert_equal(URI.parse('http://foo/bar'), URI.join('http://foo', 'bar'))
# assert_equal(URI.parse('http://foo/bar/'), URI.join('http://foo', 'bar/'))
# 
# assert_equal(URI.parse('http://foo/baz'), URI.join('http://foo', 'bar', 'baz'))
# assert_equal(URI.parse('http://foo/baz'), URI.join('http://foo', 'bar', '/baz'))
# assert_equal(URI.parse('http://foo/baz/'), URI.join('http://foo', 'bar', '/baz/'))
# assert_equal(URI.parse('http://foo/bar/baz'), URI.join('http://foo', 'bar/', 'baz'))
# assert_equal(URI.parse('http://foo/hoge'), URI.join('http://foo', 'bar', 'baz', 'hoge'))
# 
# assert_equal(URI.parse('http://foo/bar/baz'), URI.join('http://foo', 'bar/baz'))
# assert_equal(URI.parse('http://foo/bar/hoge'), URI.join('http://foo', 'bar/baz', 'hoge'))
# assert_equal(URI.parse('http://foo/bar/baz/hoge'), URI.join('http://foo', 'bar/baz/', 'hoge'))
# assert_equal(URI.parse('http://foo/hoge'), URI.join('http://foo', 'bar/baz', '/hoge'))
# assert_equal(URI.parse('http://foo/bar/hoge'), URI.join('http://foo', 'bar/baz', 'hoge'))
# assert_equal(URI.parse('http://foo/bar/baz/hoge'), URI.join('http://foo', 'bar/baz/', 'hoge'))
# assert_equal(URI.parse('http://foo/hoge'), URI.join('http://foo', 'bar/baz', '/hoge'))
