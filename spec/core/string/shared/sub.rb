shared :string_sub_bang_frozen_raises do |error|
  it "raises a #{error} when self is frozen" do
    s = "hello"
    s.freeze
    
    s.sub!(/ROAR/) { "x" } # ok
    lambda { s.sub!(/e/) { "e" } }.should raise_error(error)
    lambda { s.sub!(/[aeiou]/) { '*' } }.should raise_error(error)
  end
end
