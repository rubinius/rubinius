class Eeek
end

class Go < Eeek
end

class Blah
    def self.inherited(sub)
        puts "I am #{self}, #{sub} is my child."
    end
end

class Bleh < Blah
end
