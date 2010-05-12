class Blah

  def initialize(day)
    @day = day
  end

  define_method :fooballs do |name|
    puts "#{name} loves fooballs on #{@day}"
  end
end

b = Blah.new 'tuesday'

b.fooballs "jake"

# OUTPUT
# jake loves fooballs on tuesday

