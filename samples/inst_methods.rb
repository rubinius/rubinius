class Blah
    def gogo
        true
    end
end

meth = Blah.methods
p meth
p meth.keys
p meth.entries
p meth.keys.to_a
p meth.to_a
p meth.to_a(false)
puts "names"
p meth.names(false)
