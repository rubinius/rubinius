class Blah
    self.instance_fields = 10
    attr_reader :blah
    attr_writer :blah
    attr_accessor :bleh

    index_accessor :special, 8

    def go(a, *b)
        1
    end
end

puts "creating a blah."

b = Blah.new
b.blah = 9
puts "Setting as go."
b.blah = "go"
b.bleh = :sym
p b.blah
p b.bleh

p b.special
b.special = "go"
p b.special
