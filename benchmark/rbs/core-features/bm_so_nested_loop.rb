# $Id: nestedloop-ruby.code,v 1.4 2004/11/13 07:42:22 bfulgham Exp $
# http://www.bagley.org/~doug/shootout/
# from Avi Bryant

def nested_loop(n)
  x = 0
  n.times do
    n.times do
      n.times do
        n.times do
          n.times do
            n.times do
              x += 1
            end
          end
        end
      end
    end
  end
  return x
end

Bench.run [5, 10, 15] do |n|
  nested_loop n
end
