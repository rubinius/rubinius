---
layout: doc_en
title: Scripts
previous: Ruby
previous_url: ruby
next: Methods
next_url: ruby/methods
review: true
---

A ruby script can be a stand alone ruby program or file that is usually executed
in a command prompt, or that is part of a larger ruby application. A ruby script
ends with the *.rb file extension.

Example:

Consider the following ruby script 'hello.rb'

    puts "Hello World"

The above is a simple example of a ruby script that prints out the text "Hello World"
onto the screen. You run the script by going into the directory where the file is stored,
using your terminal, and type "ruby hello.rb" or "rbx hello.rb" if rubinius is installed.
In real life you might not need a script like the hello.rb but might need something like
the basic_math.rb ruby script below.

Consider the following ruby script 'basic_math.rb'


    def add(first_number, second_number)
      first_number.to_i + second_number.to_i
    end

    def subtract(large_number, small_number)
      large_number.to_i - small_number.to_i
    end

    def multiply(first_number, second_number)
      first_number.to_i * second_number.to_i
    end

    def divide(large_number, small_number)
      large_number.to_f / small_number.to_f
    end

    puts "---Calling addition---"
    puts "Please enter two numbers to add"

    puts "First Number:"
    first_number = gets.chomp

    puts "Second Number: "
    second_number = gets.chomp
    puts "#{first_number} + #{second_number} = #{add(first_number, second_number)}"


As the script name says [basic_math.rb], this is a ruby script that performs simple
mathematical calculation of addition, subtraction, multiplication and division using
the methods that are named accordingly. Don't worry if you don't understand
the keywords and structure at the moment, we'll talk about these in the following chapters.
For now, know that this is one way to create and use a ruby script. For the sake of demonstration 
we have kept the examples very short and simple. For further information please have a look at 
the offical ruby language documentation page <a href="http://www.ruby-lang.org/en/documentation/" target="_blank">here</a>

