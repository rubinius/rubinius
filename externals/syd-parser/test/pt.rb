require 'rubygems'
require 'parse_tree'
require 'pp'

class Blah
    def blah(*aoeu)
        blah do |a,b|
            c = a + b

            go do |d|
                c = c + d
            end
        end
    end
end

pt = ParseTree.new

pp pt.parse_tree(Blah)
