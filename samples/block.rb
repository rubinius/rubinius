=begin
def show_this
    10.times {
        yield
    }
end
=end
self.show_this(1,2) do |a,b|
    8 + 9
    puts "hello"
end
