class Blah
def blah
    bleh { return }
    puts "SHOULD NOT BE HERE."
end

def bleh
    yield
end
end

b = Blah.new
b.blah
puts "DONE."
