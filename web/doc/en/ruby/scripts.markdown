---
layout: doc_en
title: Scripts
previous: Ruby
previous_url: ruby
next: Methods
next_url: ruby/methods
review: true
---

The best way to start writing Ruby code is to write it into a file called 
a *script*. A script is just a file which contain your code ; once you have
write your script, you can execute it to see the result. 

So, in a folder, create a file called as you want, such as `main.rb`, then
open it with your favorite code editor such as Vim, TextMate or Sublime Text.
Once the file is open, the _first_ thing to put in your script is the 
_shebang_. A shebang is just a line which tell your computer which kind of 
file is your program. 

    
    #!/usr/bin/env rbx
    # You can put ruby code as you did usually
    puts "Hello world !"

It's important to put the sebang on the first line of your script. By this way,
we can now just execute our file instead of call the `rbx` command followed
by the name of your script. Before executing our script, we have to give the 
execution rights on our file. So in a shell, type:


    
    $ chmod u+x main.rb


The `chmod` command is called just once on your file. You don't have to execute
it each time you change your file or execute it. So now, you can run your script 
just calling:

    
 		$ ./main.rb


In your script, now, you can put Ruby code like you did when you use MRI
for example. In fact, the shebang is optionnal. You can execute your script
passing its name as an argument of the `rbx` command:

    
    $ rbx main.rb

If your prefer this kind of approach, you have to know that you don't have
to give the execution rights on the file. 