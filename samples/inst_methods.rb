class Blah
    def gogo
        true
    end
end

p Blah.instance_methods(true)
p Blah.instance_methods(false)
