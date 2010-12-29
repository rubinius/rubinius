---
layout: doc_en
title: Constants
previous: Methods
previous_url: ruby/methods
next: Classes & Modules
next_url: ruby/classes-and-modules
review: true
---

A ruby constant is a variable whose value shouldn't be changed for the duration 
of the program. A constant is used to hold known values, for example; In maths 
the value of Pi is 3.14159265358979323846, so in ruby we can declare a Pi constant 
like such;

    Pi = 3.14159265358979323846

To declare a ruby constant we start the name with an upper-case alphabet as shown above, 
however by convention the entire constant name is all upper-cased like this.

    PI = 3.14159265358979323846 

Note:

* Constants defined within a class or module may be accessed anywhere within the class or module.
* Constants shouldn't be defined in methods.
* Every class is a constant.
* Constants can be reassigned a value after initialization however that will generate an error.
* Trying to access an un-initialized constant will raise a NameError exception.

For more information please refer to the ruby language documentation page 
<a href="http://www.ruby-lang.org/en/documentation/" target="_blank">here</a>.
