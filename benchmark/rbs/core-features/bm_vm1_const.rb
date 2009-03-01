Const = 1
i = 0

Bench.run [30_000_000] do |n|
  while i < n # while loop 1
    i+= 1
    j = Const
    k = Const
  end
end
