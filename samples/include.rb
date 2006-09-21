module Blah
    def eek
        puts "eek!"
    end
end

p Blah

class Bleh
    p self.superclass
    include Blah
end

p Bleh.ancestors
p Bleh.superclass
