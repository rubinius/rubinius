require File.dirname(__FILE__) + '/../spec_helper'

# class methods
# rfc2822, httpdate, xmlschema, iso8601

require "time"

context "Time class method" do
  # rfc2822
  
  specify "rfc2822 should parse RFC-822 strings" do
    t1 = (Time.utc(1976, 8, 26, 14, 30) + 4 * 3600)
    t2 = Time.rfc2822("26 Aug 76 14:30 EDT")
    t1.should == t2
    
    t3 = Time.utc(1976, 8, 27, 9, 32) + 7 * 3600
    t4 = Time.rfc2822("27 Aug 76 09:32 PDT")
    t3.should == t4
  end
  
  specify "rfc2822 should parse RFC-2822 strings" do
    t1 = Time.utc(1997, 11, 21, 9, 55, 6) + 6 * 3600
    t2 = Time.rfc2822("Fri, 21 Nov 1997 09:55:06 -0600")
    t1.should == t2
    
    t3 = Time.utc(2003, 7, 1, 10, 52, 37) - 2 * 3600
    t4 = Time.rfc2822("Tue, 1 Jul 2003 10:52:37 +0200")
    t3.should == t4
    
    t5 = Time.utc(1997, 11, 21, 10, 1, 10) + 6 * 3600
    t6 = Time.rfc2822("Fri, 21 Nov 1997 10:01:10 -0600")
    t5.should == t6
    
    t7 = Time.utc(1997, 11, 21, 11, 0, 0) + 6 * 3600
    t8 = Time.rfc2822("Fri, 21 Nov 1997 11:00:00 -0600")
    t7.should == t8
    
    t9 = Time.utc(1997, 11, 24, 14, 22, 1) + 8 * 3600
    t10 = Time.rfc2822("Mon, 24 Nov 1997 14:22:01 -0800")
    t9.should == t10
    
    begin
      Time.at(-1)
    rescue ArgumentError
      # ignore
    else
      t11 = Time.utc(1969, 2, 13, 23, 32, 54) + 3 * 3600 + 30 * 60
      t12 = Time.rfc2822("Thu, 13 Feb 1969 23:32:54 -0330")
      t11.should == t12
      
      t13 = Time.utc(1969, 2, 13, 23, 32, 0) + 3 * 3600 + 30 * 60
      t14 = Time.rfc2822(" Thu,
      13
        Feb
          1969
      23:32
               -0330 (Newfoundland Time)")
      t13.should == t14
    end
    
    t15 = Time.utc(1997, 11, 21, 9, 55, 6)
    t16 = Time.rfc2822("21 Nov 97 09:55:06 GMT")
    t15.should == t16
    
    t17 = Time.utc(1997, 11, 21, 9, 55, 6) + 6 * 3600
    t18 = Time.rfc2822("Fri, 21 Nov 1997 09 :   55  :  06 -0600")
    t17.should == t18
    
    should_raise(ArgumentError) do
      # inner comment is not supported.
      Time.rfc2822("Fri, 21 Nov 1997 09(comment):   55  :  06 -0600")
    end
  end
  
  specify "rfc822 should be an alias of rfc2822" do
    Time.rfc822("Fri, 21 Nov 1997 09:55:06 -0600").should == Time.rfc2822("Fri, 21 Nov 1997 09:55:06 -0600")
  end
  
  # httpdate
  
  specify "httpdate should parse RFC-2616 strings" do
    t = Time.utc(1994, 11, 6, 8, 49, 37)
    
    t.should == Time.httpdate("Sun, 06 Nov 1994 08:49:37 GMT")
    
    # relies on Time.parse (not yet implemented)
    # t.should == Time.httpdate("Sunday, 06-Nov-94 08:49:37 GMT")
    
    t.should == Time.httpdate("Sun Nov  6 08:49:37 1994")
    
    Time.utc(1995, 11, 15, 6, 25, 24).should == Time.httpdate("Wed, 15 Nov 1995 06:25:24 GMT")
    
    Time.utc(1995, 11, 15, 4, 58, 8).should == Time.httpdate("Wed, 15 Nov 1995 04:58:08 GMT")
    
    Time.utc(1994, 11, 15, 8, 12, 31).should == Time.httpdate("Tue, 15 Nov 1994 08:12:31 GMT")
    
    Time.utc(1994, 12, 1, 16, 0, 0).should == Time.httpdate("Thu, 01 Dec 1994 16:00:00 GMT")
                 
    Time.utc(1994, 10, 29, 19, 43, 31).should == Time.httpdate("Sat, 29 Oct 1994 19:43:31 GMT")
    
    Time.utc(1994, 11, 15, 12, 45, 26).should == Time.httpdate("Tue, 15 Nov 1994 12:45:26 GMT")
    
    Time.utc(1999, 12, 31, 23, 59, 59).should == Time.httpdate("Fri, 31 Dec 1999 23:59:59 GMT")   
  end
  
  # xmlschema
  
  specify "xmlschema should parse ISO-8601 strings" do
    t = Time.utc(1985, 4, 12, 23, 20, 50, 520000)
    s = "1985-04-12T23:20:50.52Z"
    t.should == Time.xmlschema(s)
    #s.should == t.xmlschema(2)

    t = Time.utc(1996, 12, 20, 0, 39, 57)
    s = "1996-12-19T16:39:57-08:00"
    t.should == Time.xmlschema(s)
    # There is no way to generate time string with arbitrary timezone.
    s = "1996-12-20T00:39:57Z"
    t.should == Time.xmlschema(s)
    #assert_equal(s, t.xmlschema)

    t = Time.utc(1990, 12, 31, 23, 59, 60)
    s = "1990-12-31T23:59:60Z"
    t.should == Time.xmlschema(s)
    # leap second is representable only if timezone file has it.
    s = "1990-12-31T15:59:60-08:00"
    t.should == Time.xmlschema(s)

    begin
      Time.at(-1)
    rescue ArgumentError
      # ignore
    else
      t = Time.utc(1937, 1, 1, 11, 40, 27, 870000)
      s = "1937-01-01T12:00:27.87+00:20"
      t.should == Time.xmlschema(s)
    end
    
    # more
    
    # (Time.utc(1999, 5, 31, 13, 20, 0) + 5 * 3600).should == Time.xmlschema("1999-05-31T13:20:00-05:00")
    # (Time.local(2000, 1, 20, 12, 0, 0)).should == Time.xmlschema("2000-01-20T12:00:00")
    # (Time.utc(2000, 1, 20, 12, 0, 0)).should == Time.xmlschema("2000-01-20T12:00:00Z")
    # (Time.utc(2000, 1, 20, 12, 0, 0) - 12 * 3600).should == Time.xmlschema("2000-01-20T12:00:00+12:00")
    # (Time.utc(2000, 1, 20, 12, 0, 0) + 13 * 3600).should == Time.xmlschema("2000-01-20T12:00:00-13:00")
    # (Time.utc(2000, 3, 4, 23, 0, 0) - 3 * 3600).should == Time.xmlschema("2000-03-04T23:00:00+03:00")
    # (Time.utc(2000, 3, 4, 20, 0, 0)).should == Time.xmlschema("2000-03-04T20:00:00Z")
    # (Time.local(2000, 1, 15, 0, 0, 0)).should == Time.xmlschema("2000-01-15T00:00:00")
    # (Time.local(2000, 2, 15, 0, 0, 0)).should == Time.xmlschema("2000-02-15T00:00:00")
    # (Time.local(2000, 1, 15, 12, 0, 0)).should == Time.xmlschema("2000-01-15T12:00:00")
    # (Time.utc(2000, 1, 16, 12, 0, 0)).should == Time.xmlschema("2000-01-16T12:00:00Z")
    # (Time.local(2000, 1, 1, 12, 0, 0)).should == Time.xmlschema("2000-01-01T12:00:00")
    # (Time.utc(1999, 12, 31, 23, 0, 0)).should == Time.xmlschema("1999-12-31T23:00:00Z")
    # (Time.local(2000, 1, 16, 12, 0, 0)).should == Time.xmlschema("2000-01-16T12:00:00")
    # (Time.local(2000, 1, 16, 0, 0, 0)).should == Time.xmlschema("2000-01-16T00:00:00")
    # (Time.utc(2000, 1, 12, 12, 13, 14)).should == Time.xmlschema("2000-01-12T12:13:14Z")
    # (Time.utc(2001, 4, 17, 19, 23, 17, 300000)).should == Time.xmlschema("2001-04-17T19:23:17.3Z")
  end
  
  specify "iso8601 should be an alias of xmlschema" do
    Time.iso8601("1985-04-12T23:20:50.52Z").should == Time.xmlschema("1985-04-12T23:20:50.52Z")
  end
end

context "Time instance method" do
end
