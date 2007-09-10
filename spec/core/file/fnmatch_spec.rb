require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.fnmatch?" do  
  it "match entire strings" do 
    File.fnmatch?('cat',       'cat').should == true        #=> true  : match entire string
  end
  
  it "match when use a partial strings" do 
    File.fnmatch?('cat',       'category').should == false   #=> false : only match partial string
  end
  
  it "not match  when use { } because is not supported" do
    File.fnmatch?('c{at,ub}s', 'cats').should == false       #=> false : { } isn't supported
  end
  
  it "match when use the wildcard for one-character expresion (?)" do
    File.fnmatch?('c?t',     'cat').should == true          #=> true  : '?' match only 1 character
    File.fnmatch?('c??t',    'cat').should == false          #=> false : ditto
  end
  
  it "match when use the wildcard for 0 or more characters expresion (*)" do
    File.fnmatch?('c*',      'cats').should == true         #=> true  : '*' match 0 or more characters
    File.fnmatch?('c*t',     'c/a/b/t').should == true      #=> true  : ditto
  end
  
  it "match when use inclusive bracket expresion [a-z] " do
    File.fnmatch?('ca[a-z]', 'cat').should == true          #=> true  : inclusive bracket expression
  end  
  
  it "not match when the character is out of the range using an inclusive bracket expresion [x-z] " do
    File.fnmatch?('ca[x-z]', 'cat').should == false          #=> true  : inclusive bracket expression
  end
  
  it "match exclusive bracket expresion [^t] or [!t]" do
    File.fnmatch?('ca[^t]',  'cat').should == false          #=> false : exclusive bracket expression ('^' or '!')
    File.fnmatch?('ca[!t]',  'cat').should == false          #=> false : exclusive bracket expression ('^' or '!')
  end
  
  it "match case sensitive" do
    File.fnmatch?('cat', 'CAT').should == false              #=> false : case sensitive
  end
  
  it "match case insensitive" do
    File.fnmatch?('cat', 'CAT', File::FNM_CASEFOLD).should == true #=> true  : case insensitive
  end
  
  it "not match a character using the wildcard '?'" do
    File.fnmatch?('?',   '/', File::FNM_PATHNAME).should == false  #=> false : wildcard doesn't match '/' on FNM_PATHNAME
  end
  
  it "not match a character using the wildcard '*'" do
    File.fnmatch?('*',   '/', File::FNM_PATHNAME).should == false  #=> false : ditto
  end
  
  it "not match a character using a charater insede a bracket expression " do
    File.fnmatch?('[/]', '/', File::FNM_PATHNAME).should == false  #=> false : ditto
  end
  
  it "escaped wildcard becomes ordinary" do
    File.fnmatch?('\?',   '?').should == true                       #=> true  : escaped wildcard becomes ordinary
  end
  
  it "escaped ordinary remains ordinary" do
    File.fnmatch?('\a',   'a').should == true                       #=> true  : escaped ordinary remains ordinary
  end
  
  it " FNM_NOESACPE makes '\' ordinary" do
    File.fnmatch?('\a',   '\a', File::FNM_NOESCAPE).should == true  #=> true  : FNM_NOESACPE makes '\' ordinary
  end
  
  it "can escape inside bracket expression" do
    File.fnmatch?('[\?]', '?').should == true                       #=> true  : can escape inside bracket expression
  end
  
  it "wildcards doesnt match leading by default" do
    File.fnmatch?('*',   '.profile').should == false                      #=> false : wildcard doesn't match leading
    File.fnmatch?('*',   '.profile', File::FNM_DOTMATCH).should == true  #=> true    period by default.
    File.fnmatch?('.*',  '.profile').should == true                      #=> true
  end

  it "match some paths using a composite string" do
    rbfiles = '**' '/' '*.rb'                           # you don't have to do like this. just write in single string.
    File.fnmatch?(rbfiles, 'main.rb').should == false    #=> false
    File.fnmatch?(rbfiles, './main.rb').should == false  #=> false
    File.fnmatch?(rbfiles, 'lib/song.rb').should == true #=> true
    File.fnmatch?('**.rb', 'main.rb').should == true     #=> true
    File.fnmatch?('**.rb', './main.rb').should == false  #=> false
    File.fnmatch?('**.rb', 'lib/song.rb').should == true #=> true
    File.fnmatch?('*',     'dave/.profile').should == true  #=> true
  end
  
  it "match usign File::CONSTANT" do
    pattern = '*' '/' '*'
    File.fnmatch?(pattern, 'dave/.profile', File::FNM_PATHNAME)  #=> false
    File.fnmatch?(pattern, 'dave/.profile', File::FNM_PATHNAME | File::FNM_DOTMATCH) #=> true

    pattern = '**' '/' 'foo'
    File.fnmatch?(pattern, 'a/b/c/foo', File::FNM_PATHNAME)     #=> true
    File.fnmatch?(pattern, '/a/b/c/foo', File::FNM_PATHNAME)    #=> true
    File.fnmatch?(pattern, 'c:/a/b/c/foo', File::FNM_PATHNAME)  #=> true
    File.fnmatch?(pattern, 'a/.b/c/foo', File::FNM_PATHNAME)    #=> false
    File.fnmatch?(pattern, 'a/.b/c/foo', File::FNM_PATHNAME | File::FNM_DOTMATCH) #=> true
  end
  
  it "raise an exception if the argumnents are not of the correct type or are missing" do
    should_raise(ArgumentError){ File.fnmatch?(@path1, @path1, 0, 0) }
    should_raise(TypeError){ File.fnmatch?(1, @path1) }
    should_raise(TypeError){ File.fnmatch?(@path1, 1) }
    should_raise(TypeError){ File.fnmatch?(@path1, @path2, @path3) }
  end 
end

describe "File.fnmatch" do  
  it "match entire strings" do 
    File.fnmatch('cat',       'cat').should == true        #=> true  : match entire string
  end
  
  it "match when use a partial strings" do 
    File.fnmatch('cat',       'category').should == false   #=> false : only match partial string
  end
  
  it "not match  when use { } because is not supported" do
    File.fnmatch('c{at,ub}s', 'cats').should == false       #=> false : { } isn't supported
  end
  
  it "match when use the wildcard for one-character expresion (?)" do
    File.fnmatch('c?t',     'cat').should == true          #=> true  : '?' match only 1 character
    File.fnmatch('c??t',    'cat').should == false          #=> false : ditto
  end
  
  it "match when use the wildcard for 0 or more characters expresion (*)" do
    File.fnmatch('c*',      'cats').should == true         #=> true  : '*' match 0 or more characters
    File.fnmatch('c*t',     'c/a/b/t').should == true      #=> true  : ditto
  end
  
  it "match when use inclusive bracket expresion [a-z] " do
    File.fnmatch('ca[a-z]', 'cat').should == true          #=> true  : inclusive bracket expression
  end  
  
  it "not match when the character is out of the range using an inclusive bracket expresion [x-z] " do
    File.fnmatch('ca[x-z]', 'cat').should == false          #=> true  : inclusive bracket expression
  end
  
  it "match exclusive bracket expresion [^t] or [!t]" do
    File.fnmatch('ca[^t]',  'cat').should == false          #=> false : exclusive bracket expression ('^' or '!')
    File.fnmatch('ca[!t]',  'cat').should == false          #=> false : exclusive bracket expression ('^' or '!')
  end
  
  it "match case sensitive" do
    File.fnmatch('cat', 'CAT').should == false              #=> false : case sensitive
  end
  
  it "match case insensitive" do
    File.fnmatch('cat', 'CAT', File::FNM_CASEFOLD).should == true #=> true  : case insensitive
  end
  
  it "not match a character using the wildcard '?'" do
    File.fnmatch('?',   '/', File::FNM_PATHNAME).should == false  #=> false : wildcard doesn't match '/' on FNM_PATHNAME
  end
  
  it "not match a character using the wildcard '*'" do
    File.fnmatch('*',   '/', File::FNM_PATHNAME).should == false  #=> false : ditto
  end
  
  it "not match a character using a charater insede a bracket expression " do
    File.fnmatch('[/]', '/', File::FNM_PATHNAME).should == false  #=> false : ditto
  end
  
  it "escaped wildcard becomes ordinary" do
    File.fnmatch('\?',   '?').should == true                       #=> true  : escaped wildcard becomes ordinary
  end
  
  it "escaped ordinary remains ordinary" do
    File.fnmatch('\a',   'a').should == true                       #=> true  : escaped ordinary remains ordinary
  end
  
  it " FNM_NOESACPE makes '\' ordinary" do
    File.fnmatch('\a',   '\a', File::FNM_NOESCAPE).should == true  #=> true  : FNM_NOESACPE makes '\' ordinary
  end
  
  it "can escape inside bracket expression" do
    File.fnmatch('[\?]', '?').should == true                       #=> true  : can escape inside bracket expression
  end
  
  it "wildcards doesnt match leading by default" do
    File.fnmatch('*',   '.profile').should == false                      #=> false : wildcard doesn't match leading
    File.fnmatch('*',   '.profile', File::FNM_DOTMATCH).should == true  #=> true    period by default.
    File.fnmatch('.*',  '.profile').should == true                      #=> true
  end

  it "match some paths using a composite string" do
    rbfiles = '**' '/' '*.rb'                           # you don't have to do like this. just write in single string.
    File.fnmatch(rbfiles, 'main.rb').should == false    #=> false
    File.fnmatch(rbfiles, './main.rb').should == false  #=> false
    File.fnmatch(rbfiles, 'lib/song.rb').should == true #=> true
    File.fnmatch('**.rb', 'main.rb').should == true     #=> true
    File.fnmatch('**.rb', './main.rb').should == false  #=> false
    File.fnmatch('**.rb', 'lib/song.rb').should == true #=> true
    File.fnmatch('*',     'dave/.profile').should == true  #=> true
  end
  
  it "match usign File::CONSTANT" do
    pattern = '*' '/' '*'
    File.fnmatch(pattern, 'dave/.profile', File::FNM_PATHNAME)  #=> false
    File.fnmatch(pattern, 'dave/.profile', File::FNM_PATHNAME | File::FNM_DOTMATCH) #=> true

    pattern = '**' '/' 'foo'
    File.fnmatch(pattern, 'a/b/c/foo', File::FNM_PATHNAME)     #=> true
    File.fnmatch(pattern, '/a/b/c/foo', File::FNM_PATHNAME)    #=> true
    File.fnmatch(pattern, 'c:/a/b/c/foo', File::FNM_PATHNAME)  #=> true
    File.fnmatch(pattern, 'a/.b/c/foo', File::FNM_PATHNAME)    #=> false
    File.fnmatch(pattern, 'a/.b/c/foo', File::FNM_PATHNAME | File::FNM_DOTMATCH) #=> true
  end
  
  it "raise an exception if the argumnents are not of the correct type or are missing" do
    should_raise(ArgumentError){ File.fnmatch(@path1, @path1, 0, 0) }
    should_raise(TypeError){ File.fnmatch(1, @path1) }
    should_raise(TypeError){ File.fnmatch(@path1, 1) }
    should_raise(TypeError){ File.fnmatch(@path1, @path2, @path3) }
  end 
end
