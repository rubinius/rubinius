str = "hello"

ba = str.data

t1 = (ba <=> ba)

p t1

p ba.fetch_bytes(0,2)

sub = str.substring(0,2)
p sub

str2 = "hello"

p sub == str
p str == sub
p str == str
p sub == sub
p str == str2

p str.prefix?(sub)
