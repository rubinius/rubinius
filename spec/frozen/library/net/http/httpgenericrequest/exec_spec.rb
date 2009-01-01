require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require "stringio"

describe "Net::HTTPGenericRequest#exec wehn passed socket, version, path" do
  it "executes the request over the passed socket to the passed path using the passed HTTP version" do
    request = Net::HTTPGenericRequest.new("POST", true, true, "/some/path")
    socket = StringIO.new("")
    
    request.exec(socket, "1.1", "/some/path")
    socket.string.should == "POST /some/path HTTP/1.1\r\nAccept: */*\r\n\r\n"

    request = Net::HTTPGenericRequest.new("GET", true, true, "/some/path", "Content-Type" => "text/html")
    socket = StringIO.new("")

    request.exec(socket, "1.0", "/some/other/path")
    socket.string.should == "GET /some/other/path HTTP/1.0\r\nAccept: */*\r\nContent-Type: text/html\r\n\r\n"
  end
  
  describe "when a request body is set" do
    it "sets the 'Content-Type' header to 'application/x-www-form-urlencoded' unless the 'Content-Type' header is supplied" do
      request = Net::HTTPGenericRequest.new("POST", true, true, "/some/path")
      request.body = "Some Content"
      socket = StringIO.new("")

      request.exec(socket, "1.1", "/some/other/path")
      socket.string.should == "POST /some/other/path HTTP/1.1\r\nAccept: */*\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 12\r\n\r\nSome Content"
    end

    it "correctly sets the 'Content-Length' header and includes the body" do
      request = Net::HTTPGenericRequest.new("POST", true, true, "/some/path", "Content-Type" => "text/html")
      request.body = "Some Content"
      socket = StringIO.new("")

      request.exec(socket, "1.1", "/some/other/path")
      socket.string.should == "POST /some/other/path HTTP/1.1\r\nAccept: */*\r\nContent-Type: text/html\r\nContent-Length: 12\r\n\r\nSome Content"
    end
  end
  
  describe "when a body stream is set" do
    it "sets the 'Content-Type' header to 'application/x-www-form-urlencoded' unless the 'Content-Type' header is supplied" do
      request = Net::HTTPGenericRequest.new("POST", true, true, "/some/path", "Content-Length" => "10")
      request.body_stream = StringIO.new("a" * 20)
      socket = StringIO.new("")

      request.exec(socket, "1.1", "/some/other/path")
      socket.string.should == "POST /some/other/path HTTP/1.1\r\nAccept: */*\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 10\r\n\r\naaaaaaaaaaaaaaaaaaaa"
    end

    it "sends the whole stream, regardless of the 'Content-Length' header" do
      request = Net::HTTPGenericRequest.new("POST", true, true, "/some/path", "Content-Type" => "text/html", "Content-Length" => "10")
      request.body_stream = StringIO.new("a" * 20)
      socket = StringIO.new("")

      request.exec(socket, "1.1", "/some/other/path")
      socket.string.should == "POST /some/other/path HTTP/1.1\r\nAccept: */*\r\nContent-Type: text/html\r\nContent-Length: 10\r\n\r\naaaaaaaaaaaaaaaaaaaa"
    end
    
    it "sends the request in chunks of 1024 bytes when 'Transfer-Encoding' is set to 'chunked'" do
      request = Net::HTTPGenericRequest.new("POST", true, true, "/some/path", "Content-Type" => "text/html", "Transfer-Encoding" => "chunked")
      request.body_stream = StringIO.new("a" * 1024 * 2)
      socket = StringIO.new("")

      request.exec(socket, "1.1", "/some/other/path")
      socket.string.should == "POST /some/other/path HTTP/1.1\r\nAccept: */*\r\nContent-Type: text/html\r\nTransfer-Encoding: chunked\r\n\r\n400\r\n#{'a' * 1024}\r\n400\r\n#{'a' * 1024}\r\n0\r\n\r\n"
    end

    it "raises an ArgumentError when the 'Content-Length' is not set or 'Transfer-Encoding' is not set to 'chunked'" do
      request = Net::HTTPGenericRequest.new("POST", true, true, "/some/path", "Content-Type" => "text/html")
      request.body_stream = StringIO.new("Some Content")
      socket = StringIO.new("")

      lambda { request.exec(socket, "1.1", "/some/other/path") }.should raise_error(ArgumentError)
    end
  end
end
