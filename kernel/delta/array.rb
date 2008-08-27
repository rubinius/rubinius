class Array
  BASE_64_B2A = {}
  (00..25).each {|x| BASE_64_B2A[x] = (?A + x - 00).chr}
  (26..51).each {|x| BASE_64_B2A[x] = (?a + x - 26).chr}
  (52..61).each {|x| BASE_64_B2A[x] = (?0 + x - 52).chr}
  BASE_64_B2A[62] = '+'
  BASE_64_B2A[63] = '/'
end
