c6502
=====

This is a rewrite of my Simple64 project in C.

That project replicates the functionality of 6502asm.com .

Status
------

It almost works!

Current issue is that argument processing order in C is
different than in Java, which makes 'word(mem.get(pc++), mem.get(pc++))
a little awkward.
