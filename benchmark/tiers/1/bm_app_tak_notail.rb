def tak(x, y, z)
  if y < x
    tak(tak(x-1, y, z),
         tak(y-1, z, x),
         tak(z-1, x, y))
  else
    z
  end
end


def Bench.run
  tak 18, 9, 0
end
